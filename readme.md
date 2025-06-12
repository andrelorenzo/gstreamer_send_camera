### Extensiones y compilador
El proyecto se ha desarrollado en vsc, en caso de usarlo se requieren las siguientes extensiones:
* C/C++ , C/C++ Extension Pack y C/C++ Themes
* se recomienda CMake y CMake Tools
Si se sigue este procedimiento se requiere utilziar MSVC como compilador (vs 2017 en adelante)

### Descargar Gstreamer
La librería se debe descargar de https://gstreamer.freedesktop.org/download/#windows
 * Se deben instalar tanto el runtime installer como el development installer
 * Se recomienda descargarlo en C:/ pero no es obligatorio
 * los path importantes son 2:ç
    * El primero el path de la librería que apunta a la carpeta con las carpetas **bin** e **include** (en mi caso: C:\GStreamer\1.0\msvc_x86_64).
    * El segundo importante es el que apunta a la carpeta **bin** (en mi caso: C:\GStreamer\1.0\msvc_x86_64\bin)

### Compilar
Solo es necesario cambiar esta linea en el cmake con la dirección correspondiente a la carpeta de gstreamer (en caso de que sea distinta)
```c++
set(GSTREAMER_PATH "C:/GStreamer/1.0/msvc_x86_64")
```

Si se tiene CMake Tools debería aparecer un boton con el nombre **Build** abajo a la izquierda para compilar, también se debe seleccionar en la ventana de CMake el Configure con msvc (Visual Studio Community 2022 Release - amd64 en mi caso) y seleccionar **Release** (sin esto último no compilará).

En caso de no disponer de la extension de cmake el comando tiene la siguiente forma (cuidado con los paths!!!)

```bash
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ^
  --build c:/Users/ajlorenzo/Documents/vsc/gstreamer_send_camera/build ^
  --config Release ^
  --target ALL_BUILD
```

### Utilizar los distintos programas

#### Sender
Este programa simula el envió de un video por rtsp, cogiendo el path del video como argumento y mostrandolo en rtsp://127.0.0.1:8554/stream. Si no se proprociona video se coje un video de la carpeta **video_sender** con el nombre **prueba_gstreamer.mp4**.
```bash
.\sender.exe <path to video>
```

#### Multiplexer
Este es el programa principal, coje un video del stream principal y lo reenvía por los otros 2, antes de lanzar el programa se deben crear las 2 interfaces de red. **192.168.0.15 y 192.168.1.215** son las IP del ordenador donde esta corriendo el multiplexor.

```bash
.\multiplexer.exe rtsp://admin:admin123@192.168.0.10:554/live0.265 192.168.0.15 192.168.1.215
```

#### Receiver
Es el encargado de recibir el stream de video rtsp de la ip indicada

```bash
.\receiver.exe 192.168.1.215 (o 192.168.0.15)
```

#### Arquitectura probada

Se necesitan crear 2 interfaces de red distintas, se probó conectando el ordenador directamente a la cámara ip y creando la primera red (192.168.0.X), en mi caso la camara tenía la ip 192.168.0.10 y para el ordenador se cambio la ip a la 192.168.0.15 con subred /24, por otro lado se conecto otra red con otro ordenador (en nuestro caso por medio de un router inalámbrico) en la red 192.168.1.x, donde mi ordenador tenía la ip 192.168.1.215 con mascara /24.

En el ordenador conectado a la cámara se lanza el multiplexor y un receiver con la ip 192.168.0.15
En otro ordenador conectado a la red 192.168.1.x se lanza un receiver con la ip 192.168.1.215

                         +---------------------+
                         |     IP Camera       |
                         |  IP: 192.168.0.10   |
                         +----------+----------+
                                    |
                                    | (Ethernet)
                                    |
    +----------------------------------+----------------------------------+
    |                      Computer A (dual interface)                   |
    |                                                                    |
    |   Interface 1 (Ethernet):             Interface 2 (Wi-Fi):         |
    |   IP: 192.168.0.15/24                 IP: 192.168.1.215/24         |
    |   -> Multiplexer + Receiver           -> Connected to router         |
    +----------------------------------+----------------------------------+
                                    |
                                    | (Wi-Fi)
                          +---------+---------------+
                          |      Wi-Fi Router       |
                          |   Network: 192.168.1.x  |
                          +---------+---------------+
                                    |
                                    | (Wi-Fi)
                          +---------+---------+
                          |     Computer B     |
                          |  IP: 192.168.1.X   |
                          |  ↳ Receiver        |
                          +--------------------+




