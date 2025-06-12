# cmake_minimum_required(VERSION 3.27)
# project(gstreamer_static)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Ruta a tu instalación de GStreamer
# set(GSTREAMER_PATH "C:/gstreamer/1.0/msvc_x86_64")

# # Incluir cabeceras
# include_directories(${GSTREAMER_PATH}/include)
# include_directories(${GSTREAMER_PATH}/include/glib-2.0)
# include_directories(${GSTREAMER_PATH}/lib/glib-2.0/include)
# include_directories(${GSTREAMER_PATH}/include/gstreamer-1.0)

# # Ruta donde están los .a de plugins
# file(GLOB GST_STATIC_PLUGIN_LIBS "${GSTREAMER_PATH}/lib/gstreamer-1.0/*.a")

# # Bibliotecas principales necesarias (import libraries .lib)
# set(GST_CORE_LIBS
    
#     ${GSTREAMER_PATH}/lib/gstreamer-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstbase-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstvideo-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstrtp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstrtsp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstsdp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstapp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gobject-2.0.lib
#     ${GSTREAMER_PATH}/lib/glib-2.0.lib
#     ${GSTREAMER_PATH}/lib/gio-2.0.lib
#     ws2_32.lib
#     ole32.lib
# )
# # Añade estas rutas a mano si no las tienes con find_library()
# set(GST_EXTRA_LIBS
#     ${GSTREAMER_PATH}/lib/gstaudio-1.0.lib
#     ${GSTREAMER_PATH}/lib/gsttag-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstpbutils-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstnet-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstsdp-1.0.lib
#     ${GSTREAMER_PATH}/lib/soup-3.0.lib
#     ${GSTREAMER_PATH}/lib/gio-2.0.lib
#     ${GSTREAMER_PATH}/lib/jpeg.lib
#     ${GSTREAMER_PATH}/lib/intl.lib
#     ${GSTREAMER_PATH}/lib/libgstvideoconvert.a
# )
# # Ejecutable
# add_executable(receiver src/receiver.cpp)

# # Definición para plugins estáticos
# target_compile_definitions(receiver PRIVATE GST_STATIC_COMPILATION)

# # Linkea core + plugins
# target_link_libraries(receiver ${GST_CORE_LIBS} ${GST_EXTRA_LIBS} ${GST_STATIC_PLUGIN_LIBS})


cmake_minimum_required(VERSION 3.27)
project(gstreamer_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(GSTREAMER_PATH "C:/gstreamer/1.0/msvc_x86_64")
set(CMAKE_PREFIX_PATH ${GSTREAMER_PATH})


# include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${GSTREAMER_PATH}/include)

# ===== gstreamer =====
find_package(PkgConfig REQUIRED)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)
pkg_search_module(gstreamer-rtsp-server REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0>=1.4)
pkg_search_module(gstreamer-rtsp REQUIRED IMPORTED_TARGET gstreamer-rtsp-1.0>=1.4)

# 1. Simular el envió de un video RTSP
add_executable(sender src/sender.cpp)

target_link_libraries(sender
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
    PkgConfig::gstreamer-rtsp-server
    PkgConfig::gstreamer-rtsp
)


#2. Multiplexor de RTSP
add_executable(multiplexer src/multiplexer.cpp)
target_link_libraries(multiplexer
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
    PkgConfig::gstreamer-rtsp-server
    PkgConfig::gstreamer-rtsp
)

# 3. Receptor
add_executable(receiver src/receiver.cpp)
target_link_libraries(receiver
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
    PkgConfig::gstreamer-rtsp-server
    PkgConfig::gstreamer-rtsp
)


# # Directorio de librerías
# link_directories(${GSTREAMER_PATH}/lib)
# # Crear biblioteca interna "gst_static"
# add_library(gst_static STATIC IMPORTED GLOBAL)

# set(GSTREAMER_STATIC_LIBS
#     ${GSTREAMER_PATH}/lib/gstreamer-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstapp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstvideo-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstrtsp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstrtspserver-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstsdp-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstbase-1.0.lib
#     ${GSTREAMER_PATH}/lib/gstpbutils-1.0.lib

#     ${GSTREAMER_PATH}/lib/glib-2.0.lib
#     ${GSTREAMER_PATH}/lib/gobject-2.0.lib
#     ${GSTREAMER_PATH}/lib/intl.lib

#     ws2_32.lib
#     ole32.lib
#     shell32.lib
#     user32.lib
#     advapi32.lib
#     shlwapi.lib
#     strmiids.lib
# )

# # Especifica los .lib en la propiedad INTERFACE_LINK_LIBRARIES
# set_target_properties(gst_static PROPERTIES
#     IMPORTED_LOCATION "${GSTREAMER_PATH}/lib/gstreamer-1.0.lib"  # Placeholder
#     INTERFACE_LINK_LIBRARIES "${GSTREAMER_STATIC_LIBS}"
# )

# # Opcional: macro para compilación estática
# add_library(gstreamer_interface INTERFACE)
# target_compile_definitions(gstreamer_interface INTERFACE
#     -DGSTREAMER_STATIC_COMPILATION -DGLIB_STATIC_COMPILATION
# )
# target_link_libraries(gstreamer_interface INTERFACE gst_static)

# # ========== Ejecutables ==========
# add_executable(sender src/sender.cpp)
# target_link_libraries(sender PRIVATE gstreamer_interface)

# add_executable(multiplexer src/multiplexer.cpp)
# target_link_libraries(multiplexer PRIVATE gstreamer_interface)

# add_executable(receiver src/receiver.cpp)
# target_link_libraries(receiver PRIVATE gstreamer_interface)
# install(TARGETS receiver
#     RUNTIME DESTINATION dist/receiver
# )
# # ==== Instalar DLLs (manual copy desde bin) ====
# install(FILES
#     ${GSTREAMER_PATH}/bin/gstreamer-1.0-0.dll
#     ${GSTREAMER_PATH}/bin/gobject-2.0-0.dll
#     ${GSTREAMER_PATH}/bin/glib-2.0-0.dll
#     ${GSTREAMER_PATH}/bin/gmodule-2.0-0.dll
#     ${GSTREAMER_PATH}/bin/intl-8.dll
#     ${GSTREAMER_PATH}/bin/pcre2-8-0.dll
#     ${GSTREAMER_PATH}/bin/ffi-7.dll
#     DESTINATION dist/receiver/bin
# )
# install(DIRECTORY ${GSTREAMER_PATH}/lib/gstreamer-1.0
#     DESTINATION dist/receiver/lib
#     FILES_MATCHING PATTERN "*.dll"
# )
# file(WRITE ${CMAKE_BINARY_DIR}/receiver_launcher.bat
# "@echo off\r\n\
# set GST_PLUGIN_PATH=%~dp0lib\\gstreamer-1.0\r\n\
# set PATH=%~dp0bin;%PATH%\r\n\
# start \"\" receiver.exe\r\n")

# install(FILES ${CMAKE_BINARY_DIR}/receiver_launcher.bat
#     DESTINATION dist/receiver
# )
