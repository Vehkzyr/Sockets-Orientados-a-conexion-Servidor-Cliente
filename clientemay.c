#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    int socketCliente;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Comprobar argumentos, e indicamos su uso si no se cumple
    if (argc != 4) {
        printf("Uso: %s <IP> <puerto> <nombreArchivo.txt>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *nombreArchivo = argv[3];

    // Abrimos el archivo de entrada
    FILE *entrada = fopen(argv[3], "r");
    if (entrada == NULL) {
        perror("Error al abrir el archivo de entrada");
        exit(EXIT_FAILURE);
    }

    //Copio el nombre del archivo en el que se escribirá la salida
    char *nombreArchivoSalida = strdup(nombreArchivo); 

    //Elimino la extension del archivo de salida con la funcion strstr
    char *extension = strstr(nombreArchivoSalida, ".txt");
    if(extension != NULL) {
        *extension = '\0';
    }   

    //Al tener un mac no puedo solo pasar el archivo a mayusuculas pues no me lo reconoce
    //Por tanto paso el nombre a mayuscula y despues le concateno SALIDA.txt
    for(int i = 0; i < strlen(nombreArchivoSalida); i++){
        nombreArchivoSalida[i] = toupper(nombreArchivoSalida[i]);
    }

    strcat(nombreArchivoSalida, "SALIDA.txt");

    // Abrimos el archivo de entrada
    FILE *salida = fopen(nombreArchivoSalida, "w");
    if (salida == NULL) {
        perror("Error al abrir el archivo de salida");
        free(nombreArchivoSalida);
        exit(EXIT_FAILURE);
    }

    // Crear el socket
    socketCliente = socket(AF_INET, SOCK_STREAM, 0);
    if (socketCliente == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

   // Inicializamos el servidor 
    memset(&server_addr, 0, sizeof(server_addr));
    // configuramos las direcciones de familia del socket
    server_addr.sin_family = AF_INET; 
    // convertimos la dirección IP
    int ip = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);  
    //Comprobamos que la ip sea valida
    if(ip <= 0) {
        if(ip == 0)
            printf("Dirección IP no válida\n");
        else
            perror("Error al convertir la dirección IP");

        close(socketCliente);
        exit(EXIT_FAILURE);
    }
    // Establecemos el puerto desde el que escucha el servidor
    server_addr.sin_port = htons(atoi(argv[2])); 

    // Conectamos al servidor
    if (connect(socketCliente, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar");
        close(socketCliente);
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), entrada)) {
        // Enviar línea al servidor y si falla salimos del programa
        if (send(socketCliente, buffer, strlen(buffer), 0) == -1) {
            perror("Error al enviar datos");
            fclose(entrada);
            fclose(salida);
            close(socketCliente);
            exit(EXIT_FAILURE);
        }

        printf("%s\n", buffer);

        // Recibir línea convertida del servidor
        int num_bytes = recv(socketCliente, buffer, sizeof(buffer) - 1, 0);
        if (num_bytes == -1) {
            perror("Error al recibir datos");
            fclose(entrada);
            fclose(salida);
            close(socketCliente);
            exit(EXIT_FAILURE);
        }
        buffer[num_bytes] = '\0'; // Hacemos la comprobacion

        // Escribir la línea convertida al archivo de salida
        if(fputs(buffer, salida) == EOF){
            // fputs devuelve EOF en caso de error
            perror("Error al escribir en el archivo de salida");
            // Manejo de errores adicional si es necesario
        } else {
            printf("%s\n",buffer);
        }

        sleep(1); // Esperamos un segundo para lanzar otro cliente por terminal
    }

    //Cerramos los archivos
    fclose(entrada);
    fclose(salida);
    // Cerrar el socket
    close(socketCliente);

    free(nombreArchivoSalida);

    return 0;
}
