#include <gst/gst.h>
#include <gst/rtsp/rtsp.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/rtsp-server/rtsp-client.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>


int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    std::string ip = "";
    std::string pipe_str = "rtspsrc location=rtsp://" + ip + ":8554/stream latency=100 ! rtph265depay ! avdec_h265 ! autovideosink";
    
    printf("Introduzca la direccion IP de recepcion\n\t ->IP:  ");
    std::cin >> ip;
    std::cout << "IP introducida: "<< ip << std::endl;

    GstElement* pipe = nullptr;
    GError* error = nullptr;
    GstStateChangeReturn ret;

    std::cout << "Esperando a que el stream este disponible..." << std::endl;
    int dots = 1;
    while (true) {
        pipe = gst_parse_launch(pipe_str.c_str(), &error);
        if (!pipe) {
            std::cerr << "Error creando la tubería: " << (error ? error->message : "desconocido") << std::endl;
            if (error) g_error_free(error);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        ret = gst_element_set_state(pipe, GST_STATE_PLAYING);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            std::cerr << "Fallo en la conexion. Reintentando..." << std::endl;
            gst_object_unref(pipe);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        // Esperar hasta 5 segundos por cambio de estado
        GstState state;
        ret = gst_element_get_state(pipe, &state, nullptr, 5 * GST_SECOND);
        if (ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_NO_PREROLL) {
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Stream conectado correctamente con ip: " << ip <<std::endl;
            std::cout << "Puerto: 8554 y codec/decodec: 265" << std::endl;
            break;
        } else {
            std::cout << "\x1B[2J\x1B[H";
            printf("\rReintentando %.*s", dots, ".........."); // Usa hasta `dots` caracteres
            gst_element_set_state(pipe, GST_STATE_NULL);
            gst_object_unref(pipe);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            dots++;
        }
        if(dots >= 4)dots = 1;
    }
    

    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(pipe, GST_STATE_NULL);
    gst_object_unref(pipe);
    return 0;
}