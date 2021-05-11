#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <iostream>

int main(int argc, char **argv){

    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;

    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    hintsBusqueda.ai_family = AF_INET; //Solo IPV4 que es con lo que trabajamos
    hintsBusqueda.ai_socktype = SOCK_STREAM; //UDP

    int re = getaddrinfo(argv[1], argv[2], &hintsBusqueda, &resultado);

    if(re !=0){
        std::cerr<<"Error de getaddrinfo por: "<<gai_strerror(re)<<std::endl;
        return -1;
    }

    //Ponemos en el protocolo 0 para que se autoajuste
    int sock = socket(resultado->ai_family,resultado->ai_socktype,0);

    if(sock == -1){
        std::cerr<<"Error al crear el socket" <<std::endl;
        return -1;
    }

    //Unir socket con direccion
    bind(sock,resultado->ai_addr,resultado->ai_addrlen);
 
    listen(sock,16);
    
    struct sockaddr cliente;
    socklen_t  clienteLen = sizeof(cliente);

    int cliente_sock = accept(sock, &cliente, &clienteLen);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&cliente, clienteLen,
                host, NI_MAXHOST,
                serv, NI_MAXSERV,
                NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << "Conexion desde : " << host <<" En el puerto: "<< serv << std::endl;

    bool bucle=true;
    while(bucle){
        //Preparacion, máquinas que se comunican
        char buffer[80]; 

        //recepcion de alguien
        int bytesRecibidos = recv(cliente_sock,buffer,80,0);
        buffer[bytesRecibidos] = '\0';

        if(bytesRecibidos <= 0){
            std::cout<<"Fin de la conexion"<<std::endl;
            break;
        }
        std::cout << buffer << std::endl;

        send(cliente_sock, buffer,bytesRecibidos,0);
    }


    //Limpieza de ejecucion
    std::cout <<"Cerramos el servidor "<< std::endl;
    close(sock);
    freeaddrinfo(resultado);

    return 0;
}
