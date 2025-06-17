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
