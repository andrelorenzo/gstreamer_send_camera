#include <gst/rtsp-server/rtsp-server.h>
#include <gio/gio.h>
#include <filesystem>
#include <string>
#include <iostream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>  // <- Aquí se define INET_ADDRSTRLEN
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


namespace fs = std::filesystem;

std::string get_local_ip() {
#ifdef _WIN32
    WSADATA wsaData;
    char ipstr[INET_ADDRSTRLEN] = "127.0.0.1";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return "127.0.0.1";

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        WSACleanup();
        return "127.0.0.1";
    }

    struct hostent *host = gethostbyname(hostname);
    if (host == nullptr) {
        WSACleanup();
        return "127.0.0.1";
    }

    struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
    for (int i = 0; addr_list[i] != nullptr; i++) {
        std::string ip = inet_ntoa(*addr_list[i]);
        if (ip != "127.0.0.1") {
            WSACleanup();
            return ip;
        }
    }

    WSACleanup();
    return "127.0.0.1";
#else
    return "127.0.0.1"; // Fallback para otras plataformas
#endif
}

static void on_client_connected(GstRTSPServer *server, GstRTSPClient *client, gpointer user_data) {
    GstRTSPConnection *conn = gst_rtsp_client_get_connection(client);
    if (conn) {
        const gchar *ip = gst_rtsp_connection_get_ip(conn);
        if (ip)
            g_print("[Cliente] conectado desde IP: %s\n", ip);
        else
            g_print("[Cliente] conectado (IP no disponible)\n");
    } else {
        g_print("[Cliente] conectado (no se pudo obtener conexión)\n");
    }
}

int main(int argc, char **argv) {
    std::string video_path = "test.mp4";

    // Verificación de existencia del archivo
    if (!fs::exists(video_path)) {
        std::cerr << "Error: El archivo de vídeo no existe en la ruta proporcionada:\n\t" << video_path << "\n";
        return -1;
    }

    // Inicialización de GStreamer y servidor
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    GstRTSPServer *server = gst_rtsp_server_new();
    g_signal_connect(server, "client-connected", G_CALLBACK(on_client_connected), nullptr);

    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();

    std::string pipeline =
        "( filesrc location=\"" + video_path +
        "\" ! decodebin ! videoconvert ! x265enc tune=zerolatency key-int-max=30 ! h265parse ! rtph265pay name=pay0 pt=96 )";
    gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());
    gst_rtsp_media_factory_set_shared(factory, TRUE);
    gst_rtsp_mount_points_add_factory(mounts, "/live", factory);
    g_object_unref(mounts);

    if (!gst_rtsp_server_attach(server, NULL)) {
        std::cerr << "Error al iniciar el servidor RTSP\n";
        return -1;
    }

    std::string local_ip = get_local_ip();
    g_print("[Servidor] stream listo en rtsp://%s:8554/live\n", local_ip.c_str());
    g_print(std::string("[Servidor] Path de video: " + video_path + "\n").c_str());

    g_main_loop_run(loop);
    return 0;
}
