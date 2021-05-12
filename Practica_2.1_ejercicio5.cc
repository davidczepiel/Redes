#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <iostream>

int main(int argc, char **argv){

    //Sacamos la info del server
    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));
    hintsBusqueda.ai_family = AF_INET; //Solo IPV4 que es con lo que trabajamos
    hintsBusqueda.ai_socktype = SOCK_STREAM; //TCP
    int re = getaddrinfo(argv[1], argv[2], &hintsBusqueda, &resultado);

    //Comprobamos que tenemos informacion correcta
    if(re !=0){
        std::cerr<<"Error de getaddrinfo por: "<<gai_strerror(re)<<std::endl;
        return -1;
    }

    //Nos hacemos con nuestro medio de comunicacion
    int sock = socket(resultado->ai_family,resultado->ai_socktype,0);
    if(sock == -1){
        std::cerr<<"Error al crear el socket" <<std::endl;
        return -1;
    }

    //Conexion al server
    int connectionResult =connect(sock, resultado->ai_addr, resultado->ai_addrlen);
    if(connectionResult == -1){
        std::cout<<"Error al conectarse al servidor"<<std::endl;
        return -1;
    }

    // //Bucle de ejecucion
    std::string lector;
    while(true){

        std::cin>>lector;
        //Fin de la conexion
        if(lector.length() == 1 && lector == "Q"){
            break;
        }
        //Metemos el string en algo manejable por el send()
        int n = lector.length();
        char char_Comunicacion[n+1];
        strcpy(char_Comunicacion, lector.c_str());

        // //Mandamos el comando comcreto
        int bytesMandados = send(sock, char_Comunicacion,sizeof(char_Comunicacion),0);
        if(bytesMandados == -1){
            std::cout<<"Ha habido un error en el envio del comando "<<std::endl;
            return -1;
        }

        // //Recibimos la informacion del servidor
        char buffer[80]; 
        int bytesRecibidos = recv(sock,buffer,80,0);

        //Se ha producido un error al recibir los datos
        if(bytesRecibidos == -1){
            std::cout<<"Ha habido un error en la recepcion de los datos del server"<<std::endl;
            return -1;
        }

        //No se ha recibido nada, el servidor ha cerrado y pensamos que sigue en pie
        else if(bytesRecibidos == 0){
            std::cout<<"0 bytes recibidos servidor cerrado"<<std::endl;
            break;
        }
        buffer[bytesRecibidos] = '\0';
        
        std::cout<< "Se me ha devuelto: "<<buffer<<std::endl;
    }
    
    close(sock);
    freeaddrinfo(resultado);
    std::cout<<"Se ha cerrado la conexion"<<std::endl;

    return 0;
}
