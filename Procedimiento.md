### 
[CAM/QSLAM/RADIO] -----> [GCS/FLYSTATION] <---(USBC a ethernet)---> [GMV]

1) Conectar tablet a radio (esto es la primera red con la tablet en 192.168.0.x)
2) Conectar tablet a donde GMV diga (esto es la segunda red donde diga GMV en x.x.x.x)
3) En "Ver conexiones de red" comprobar que que las 2 IP esten en las redes correctas (Red 1 con la 192.168.0.x y Red 2 que este dentro de la de GMV)
4) Asegurarse que la tablet no este conectada a mas de 2 redes (por ejemplo una wifi).
5) Lanzar la app multiplexor (para ello hay que lanzar el launcher.bat en su misma carpeta)
6) La app puede tardar unos segundos la primera vez que se lanza, te dirá que IP tiene la tablet en las 2 redes a las que esta coenctada (Aquí se comprobará si esta correcto) y de donde esta cogiendo el video.
7) La ruta rtsp que sale en la aplicacion habra que copiarla y pegarla en flystation, según software hay que cambiar la url rtsp y volver a abrir la aplicacion para que consiga cojer el feed dea la nuva URL (el splitter debe estar ya lanzado)
8) La otra ruta que pone en la aplicacion será a donde se conecten los de GMV.


Existe la app de receiver y sender para probar la comunicación en caso de que algo no funcione bien.
