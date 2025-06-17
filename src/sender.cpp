
#include <gst/rtsp-server/rtsp-server.h>
#include <gio/gio.h>
#include <filesystem>
#include <string>
#include <iostream>

namespace fs = std::filesystem;
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

int main (int argc, char **argv) {
    std::string video_path = "../../video_sender/prueba_gstreamer.mp4";

    if(argc > 1){
        video_path = argv[1];
    } else {
        printf("Introduzca path de video absoluto ( \"def\" cogera el video test.mp4)\n\t->Path: ");
        std::cin >> video_path;
        if(video_path.starts_with("def")){
            video_path = "test.mp4";
        } else {
            while(!video_path.starts_with("C:/")){
                std::cout << "\x1B[2J\x1B[H";
                printf("Error en el formato, introduce un path valido \n\t ->path: ");
                std::cin >> video_path;
            }
        }
    }

    // Verificación de existencia del archivo
    if (!fs::exists(video_path)) {
        std::cerr << "Error: El archivo de vídeo no existe en la ruta proporcionada:\n\t" << video_path << "\n";
        return -1;
    }

    // Inicialización de GStreamer y servidor
    gst_init (&argc, &argv);
   
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    GstRTSPServer *server = gst_rtsp_server_new ();
    g_signal_connect(server, "client-connected", G_CALLBACK(on_client_connected), nullptr);

    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points (server);
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new ();

    std::string pipeline =
        "( filesrc location=\"" + video_path +
        "\" ! decodebin ! videoconvert ! x265enc tune=zerolatency key-int-max=30 ! h265parse ! rtph265pay name=pay0 pt=96 )";
    gst_rtsp_media_factory_set_launch (factory, pipeline.c_str());
    gst_rtsp_media_factory_set_shared (factory, TRUE);
    gst_rtsp_mount_points_add_factory (mounts, "/stream", factory);
    g_object_unref (mounts);

    if (!gst_rtsp_server_attach (server, NULL)) {
        std::cerr << "Error al iniciar el servidor RTSP\n";
        return -1;
    }

    // Información para el usuario
    g_print ("[Servidor] stream ready at rtsp://192.168.0.20:8554/stream\n");
    g_print(std::string("[Servidor] Path de video: " + video_path + "\n").c_str());

    g_main_loop_run (loop);
    return 0;
}
