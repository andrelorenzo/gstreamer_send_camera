#include <gst/rtsp-server/rtsp-server.h>
#include <string>
#include <iostream>
#include <vector>

#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

static void on_client_connected(GstRTSPServer *server, GstRTSPClient *client, gpointer user_data) {
    GstRTSPConnection *conn = gst_rtsp_client_get_connection(client);
    if (conn) {
        const gchar *ip = gst_rtsp_connection_get_ip(conn);
        if (ip)
            g_print("[Cliente] conectado desde IP: %s\n", ip);
        else
            g_print("[Cliente] conectado (IP no disponible)\n");
    } else {
        g_print("[Cliente] No conectado (no se pudo obtener conexión)\n");
    }
}
// #define nob_shift(xs, xs_sz) (NOB_ASSERT((xs_sz) > 0), (xs_sz)--, *(xs)++)
bool detectar_ips_redes_privadas(std::string &ip1, std::string &ip2) {
    ULONG bufferSize = 15000;
    std::vector<char> buffer(bufferSize);
    IP_ADAPTER_ADDRESSES* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, adapters, &bufferSize) != NO_ERROR) {
        return false;
    }

    std::vector<std::string> ips_detectadas;
    for (IP_ADAPTER_ADDRESSES* adapter = adapters; adapter; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp)
            continue;

        for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; address; address = address->Next) {
            SOCKADDR_IN* sa_in = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);
            char strbuf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(sa_in->sin_addr), strbuf, INET_ADDRSTRLEN);
            std::string ip = strbuf;
            if (ip.rfind("127.0.0", 0) != 0) {
                printf("Interfaz detectada: %s\n",ip.c_str());
                ips_detectadas.push_back(ip);
            }
        }
        
    }

    if (ips_detectadas.size() >= 2) {
        ip1 = ips_detectadas[0];
        ip2 = ips_detectadas[1];
        return true;
    } else {
        return false;
    }
}

// Callback para imprimir errores del pipeline
static void on_media_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media, gpointer user_data) {
    GstElement *element = gst_rtsp_media_get_element(media);
    GstBus *bus = gst_element_get_bus(element);
    gst_bus_add_watch(bus, [](GstBus *, GstMessage *msg, gpointer) -> gboolean {
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
            GError *err;
            gchar *debug;
            gst_message_parse_error(msg, &err, &debug);
            g_printerr("Pipeline error: %s\n", err->message);
            g_free(debug);
            g_error_free(err);
        }
        return TRUE;
    }, nullptr);
    gst_object_unref(bus);
    gst_object_unref(element);
}

// Funcion para crear un servidor RTSP en una IP concreta
GstRTSPServer* crear_servidor(const std::string& ip, const std::string& pipeline) {
    auto *server = gst_rtsp_server_new();
    g_signal_connect(server, "client-connected", G_CALLBACK(on_client_connected), nullptr);
    gst_rtsp_server_set_address(server, ip.c_str());
    gst_rtsp_server_set_service(server, "8554");

    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();

    gst_rtsp_media_factory_set_launch(factory, pipeline.c_str());
    gst_rtsp_media_factory_set_shared(factory, TRUE);
    g_signal_connect(factory, "media-configure", (GCallback)on_media_configure, nullptr);

    gst_rtsp_mount_points_add_factory(mounts, "/stream", factory);
    g_object_unref(mounts);

    if (!gst_rtsp_server_attach(server, nullptr)) {
        std::cerr << "Error al iniciar el servidor RTSP en " << ip << "\n";
        return nullptr;
    }

    return server;
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    std::string cam_url = "rtsp://admin:admin123@192.168.0.10:554/live0.265";

    std::string ip_eth0, ip_eth1;
    if (!detectar_ips_redes_privadas(ip_eth0, ip_eth1)) {
        std::cerr << "Se han detectado menos o más de 2 interfaces de red en este dispositivo\n";
        std::cerr << "El dispositivo debe estar conectado SOLO a 2 interfaces red distintas\n";
        return -1;
    }
    std::string pipeline =
        "rtspsrc location=" + cam_url + " latency=100 ! "
        "rtph265depay ! h265parse ! rtph265pay name=pay0 pt=96";


    auto server0 = crear_servidor(ip_eth0, pipeline);
    auto server1 = crear_servidor(ip_eth1, pipeline);

    if (!server0 || !server1) {
        std::cerr << "No se pudo iniciar uno o ambos servidores.\n";
        return -1;
    }

      // debug solo
    std::cout << "\x1B[2J\x1B[H";
    std::cout << "[Servidor] RTSP activo en ambas redes:\n";
    std::cout << "\trtsp://" << ip_eth0 << ":8554/stream\n";
    std::cout << "\trtsp://" << ip_eth1 << ":8554/stream\n";
    std::cout << "[Servidor] capturando stream de video desde: \n\t";
    std::cout << cam_url << std::endl << std::endl;
    std::cout << "[Servidor] Puerto: 8554 y codec/decodec: 265" << std::endl;
    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);
    return 0;
}
