# Sockets Orientados a conexion: Servidor/Cliente

## Servidor de Mayúsculas

Este proyecto consta de un servidor (servidormay.c) y un cliente (clientemay.c). El cliente lee un archivo de texto de entrada y lo envía línea por línea al servidor. El servidor convierte cada línea a mayúsculas y lo envía de vuelta al cliente. El cliente escribe las líneas convertidas en un archivo de salida con el mismo nombre pero todo en mayúsculas.
Compilación

Para compilar los programas, sigue los siguientes pasos:

    gcc -o clientemay ./clientemay.c
    gcc -o servidormay ./servidormay.c
   

## Ejecución

1. Iniciar el Servidor:
   Abre una terminal y ejecuta:
    ```
    ./servidormay 12345
    ```
    (Puedes reemplazar 12345 por el puerto de tu elección)

2. Iniciar el Cliente:

    En una terminal diferente, ejecuta:
     ```
    ./clientemay 127.0.0.1 12345 archivo.txt
     ```
    (Asegúrate de reemplazar 12345 con el mismo puerto que usaste para el servidor y archivo.txt por el nombre del archivo que desees procesar.)


