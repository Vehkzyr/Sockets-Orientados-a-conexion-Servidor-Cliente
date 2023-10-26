#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>      
#include <stdlib.h>     
#include <unistd.h>      
#include <arpa/inet.h>  
#include <ctype.h>
#include <signal.h>

/**
 * Hace que el proceso padre espere a los hijos
 * @param senial - Señal a manejar
*/
void manejador ( int senial ){
    while ( waitpid (WAIT_ANY, NULL, WNOHANG)>0) ;
}

int main(int argc, char *argv[]) {
    
     // Verifica que se proporciona el número correcto de argumentos
    if (argc != 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    // Registrar la función manejador para manejar la señal SIGCHLD
    signal(SIGCHLD, manejador);

    //Debe escuchar todas sus interfaces usando un puertp indicado por linea de comandos
    int puerto = atoi(argv[1]);

    //declaramos el socket del cliente y el de escucha
    int socketEscucha, socketCliente;

    //asignamos las direcciones al servidor y cliente
    struct sockaddr_in server_addr, cliente_addr;
    socklen_t cliente_len;

    //Creamos el socket de escucha
    socketEscucha = socket(AF_INET,SOCK_STREAM,0);

    if(socketEscucha < 0){
        perror("Error en la creacion del socket de escucha");
        return 1;
    }

    //Inicializamos el servidor 
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET; //configuramos las direcciones de familia del socket
    server_addr.sin_addr.s_addr = INADDR_ANY; //Establecemos la direccion a escuchar, en este acaso cualquiera
    server_addr.sin_port = htons(puerto); //Establecemos el puerto desde el que escucha el servidor

    //Vinculamos el socket de escucha
    if(bind(socketEscucha,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("Error al vincular el socket\n");
        close(socketEscucha);
        return 1;
    }
    
    //Ponemos el socket de escucha a escuchar
    listen(socketEscucha,5);

    //Ponemos el 1 poruqe tienen que estar llegando solicitudes
    while(1){
        cliente_len = sizeof(cliente_addr);
        //aceptamos la conexion
        socketCliente = accept(socketEscucha,(struct sockaddr *) &cliente_addr, &cliente_len);
        if(socketCliente < 0){
            perror("Error al aceptar la conexión");
            continue;
        }

        //Creamos un proceso hijo
        pid_t pid = fork();

        if(pid == 0 ){ //Proceso hijo
            //El proceso hijo cierra el socket de escucha
            close(socketEscucha);

            //Recibimos las lineas desde el servidor 
            int n = 0;
            char buffer[1024];

            while ((n = recv(socketCliente, buffer, sizeof(buffer), 0)) > 0){
                buffer[n] = '\0'; // Hacemos la comprobacion

            // Convertimos la linea a mayusculas con la funcion toupper
                    for (int i = 0; i < n; ++i) {
                    // Comprobamos que no sea un espacio
                    if(buffer[i] != '\0'){
                      buffer[i] = toupper(buffer[i]);
                    }
                }

                // Enviamos la linea al cliente de vuelta
                if (send(socketCliente, buffer, n, 0) == -1) {
                    perror("Error al enviar datos");
                    break;
                }

            }
            //Cerramos el socket del cliente
            close(socketCliente);
            //Terminamos el proceso hijo
            exit(EXIT_SUCCESS);
        }
        else if(pid > 0 ){ //Proceso padre
            //El proceso padre cierra el socket de escucha
            close(socketCliente);
        }
        else{ //Error al crear el proceso hijo
            perror("Error al crear el proceso hijo");
            close(socketEscucha);
            return 1;
        }

    }

    return 0;
}