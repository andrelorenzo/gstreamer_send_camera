#include <gst/rtsp-server/rtsp-server.h>
#include <string>
#include <iostream>
#include <vector>

#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")


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
    printf("Buscando interfaces activas: \n");
    for (IP_ADAPTER_ADDRESSES* adapter = adapters; adapter; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp)
            continue;
        for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; address; address = address->Next) {
            SOCKADDR_IN* sa_in = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);
            char strbuf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(sa_in->sin_addr), strbuf, INET_ADDRSTRLEN);
            std::string ip = strbuf;
            if(ip != "127.0.0.1"){
                printf("\t%s\n",ip.c_str());
                ips_detectadas.push_back(ip);
            }
        }
        
    }
    printf("----------------------------------\n");
    
    if (ips_detectadas.size() >= 2) {
        if(ips_detectadas.size() == 2){
            ip1 = ips_detectadas[0];
            ip2 = ips_detectadas[1];
        }else{
            std::cout << "\x1B[2J\x1B[H";
            int i = 0;
            for(i = 0; i < ips_detectadas.size(); i++){
                printf("Interfaz detectada [%i]: %s\n",(i+1),ips_detectadas[i].c_str());
            }
            printf("De las %i interfaces detectadas selecciona 2: \n",(i));
            std::cout << "\tIntroduce la ip para la interfaz de red 1: ";
            std::cin >> ip1;
            std::cout << std::endl;
            std::cout << "\tIntroduce la ip para la interfaz de red 2: ";
            std::cin >> ip2;
            std::cout << std::endl;

        }
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
    std::string resp = "";
    std::cout << "Test de camara?(s/n): ";
    std::cin>> resp;
    std::string cam_url = "rtsp://admin:admin123@192.168.0.10:554/live0.265";
    if(resp == "s"){
        cam_url = "rtsp://127.0.0.1:8554/live";
    }

    std::string ip_eth0, ip_eth1;
    if (!detectar_ips_redes_privadas(ip_eth0, ip_eth1)) {
        std::cerr << "Se han detectado menos de 2 interfaces de red en este dispositivo\n";
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
