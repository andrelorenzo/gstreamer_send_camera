#include <gst.h>
#include <rtsp/rtsp.h>
#include <rtsp-server/rtsp-client.h>
#include <rtsp-server/rtsp-server.h>
#include <string>
#include <iostream>


int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    std::string ip = "192.168.1.15";
    
    if (argc < 2) {
        g_printerr("Uso: %s <rtsp_url>\n", argv[0]);
        // return -1;
    }else{
        ip = std::string(argv[1]);
    }

    std::string pipeline = "rtspsrc location= rtsp://" + ip +":8554/stream latency=100 ! rtph265depay ! avdec_h265 ! autovideosink";

    GstElement *pipe = gst_parse_launch(pipeline.c_str(), nullptr);
    gst_element_set_state(pipe, GST_STATE_PLAYING);

    GMainLoop *loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(pipe, GST_STATE_NULL);
    gst_object_unref(pipe);
    return 0;
}