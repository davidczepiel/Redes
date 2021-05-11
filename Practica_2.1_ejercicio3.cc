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
    hintsBusqueda.ai_socktype = SOCK_DGRAM; //UDP
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

    //Mandamos el comando comcreto
    int bytesMandados = sendto(sock, argv[3],1,0,resultado->ai_addr,resultado->ai_addrlen);
    if(bytesMandados == -1){
        std::cout<<"Ha habido un error en el envio del comando "<<std::endl;
        return -1;
    }

    //Recibimos la informacion del servidor
    char buffer[80]; 
    int bytesRecibidos = recvfrom(sock,buffer,80,0,resultado->ai_addr,&resultado->ai_addrlen);
    if(bytesRecibidos == -1){
        std::cout<<"Ha habido un error en la recepcion de los datos del server"<<std::endl;
        return -1;
    }
    buffer[bytesRecibidos] = '\0';
    
    std::cout<< "Se me ha devuelto: "<<buffer<<std::endl;
    
    close(sock);
    freeaddrinfo(resultado);

    return 0;
}
