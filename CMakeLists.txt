cmake_minimum_required(VERSION 3.16)
project(gstreamer_static_example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ruta de GStreamer
set(GSTREAMER_PATH "C:/gstreamer/1.0/mingw_x86_64")

# Incluir directorios
include_directories(
    ${GSTREAMER_PATH}/include
    ${GSTREAMER_PATH}/include/gstreamer-1.0
    ${GSTREAMER_PATH}/include/glib-2.0
    ${GSTREAMER_PATH}/lib/glib-2.0/include
)

# Añadir directorio de bibliotecas
link_directories(${GSTREAMER_PATH}/lib)

# Buscar todos los plugins .a (estáticos)
file(GLOB GST_STATIC_PLUGINS "${GSTREAMER_PATH}/lib/gstreamer-1.0/libgst*.a")

# Bibliotecas principales (linking estático parcial)
set(GST_CORE_LIBS
    gstapp-1.0
    gstrtsp-1.0
    gstrtp-1.0
    gstsdp-1.0
    gstvideo-1.0
    gstbase-1.0
    gstreamer-1.0
    gobject-2.0
    glib-2.0
    gio-2.0
    intl
    jpeg
    ws2_32  # sockets en Windows
    ole32   # OLE/COM
)

# Construir el ejecutable
add_executable(receiver src/receiver.cpp)

# Definición para registrar plugins estáticos
target_compile_definitions(receiver PRIVATE GST_STATIC_COMPILATION)

# Link a las libs principales y plugins estáticos
target_link_libraries(receiver
    ${GST_CORE_LIBS}
    ${GST_STATIC_PLUGINS}
)
