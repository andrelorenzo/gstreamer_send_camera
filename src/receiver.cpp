#include <gst/gst.h>
#include <gst/rtsp/rtsp.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/rtsp-server/rtsp-client.h>
#include <string>
#include <iostream>



int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    std::string ip = "192.168.1.15";
    
    if (argc < 2) {
        printf("Introduzca la direccion IP de recepción\n\t ->IP:  ");
        std::cin >> ip;
        while(!ip.starts_with("192.168.") && !ip.starts_with("127.0")){
            std::cout << "\x1B[2J\x1B[H";
            printf("Error en el formato, introduce una IP valida\n\t ->IP: ");
            std::cin >> ip;
        }
        printf("IP correcta, esperando stream...\n");
    }else{
        ip = std::string(argv[1]);
    }

    std::string pipeline = "rtspsrc location=rtsp://" + ip + ":8554/stream latency=100 ! rtph265depay ! avdec_h265 ! autovideosink";
    GError* error = nullptr;
    GstElement *pipe = gst_parse_launch(pipeline.c_str(), &error);

    if (!pipe) {
        std::cerr << "Error al crear la tubería GStreamer:\n";
        if (error) {
            std::cerr << error->message << std::endl;
            g_error_free(error);
        }
        return -1;
    }
    gst_element_set_state(pipe, GST_STATE_PLAYING);

    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(pipe, GST_STATE_NULL);
    gst_object_unref(pipe);
    return 0;
}