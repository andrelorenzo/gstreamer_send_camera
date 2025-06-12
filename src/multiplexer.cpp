#include <gst/gst.h>
#include <gst/rtsp/rtsp.h>
#include <gst/rtsp-server/rtsp-client.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <string>
#include <iostream>


// #define nob_shift(xs, xs_sz) (NOB_ASSERT((xs_sz) > 0), (xs_sz)--, *(xs)++)

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

// Función para crear un servidor RTSP en una IP concreta
GstRTSPServer* crear_servidor(const std::string& ip, const std::string& pipeline) {
    auto *server = gst_rtsp_server_new();
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
    std::string ip_eth0 = "192.168.0.15";
    std::string ip_eth1 = "192.168.1.215";

    if(argc < 4){
        printf("Usage:\n\t <ip-camera> <ip-interfaz-1> <ip-interfaz-2> \n");
        return 0;
    }else{
        cam_url = std::string(argv[1]);
        ip_eth0 = std::string(argv[2]);
        ip_eth1 = std::string(argv[3]);
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
    std::cout << "Servidor RTSP activo en ambas redes:\n";
    std::cout << "rtsp://" << ip_eth0 << ":8554/stream\n";
    std::cout << "rtsp://" << ip_eth1 << ":8554/stream\n";
    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);
    return 0;
}
