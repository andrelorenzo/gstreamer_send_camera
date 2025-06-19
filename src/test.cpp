#include <gst/rtsp-server/rtsp-server.h>
#include <string>
#include <iostream>
#include <vector>

#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")



int main(int argc, char ** argv){
    /* intitialize the structures and other gstreamer stuff, if argc and ragv are provided, then you can perse them with its internall tools*/
    gst_init(&argc, &argv); 
    



}