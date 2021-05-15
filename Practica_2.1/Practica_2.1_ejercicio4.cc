#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <iostream>

int main(int argc, char **argv){

    //Error por si se ejecuta el programa de manera no adecuada
    if(argc !=3){
        std::cout<<"El programa debe recibir una direccion IP y un puerto"<<std::endl;
        return -1;
    }

    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    hintsBusqueda.ai_family = AF_INET; //Solo IPV4 
    hintsBusqueda.ai_socktype = SOCK_STREAM; //TCP

    //Sacamos informacion o recibimos error
    int re = getaddrinfo(argv[1], argv[2], &hintsBusqueda, &resultado);
    if(re !=0){
        std::cerr<<"Error de getaddrinfo por: "<<gai_strerror(re)<<std::endl;
        return -1;
    }

    //Creamos el socket para comunicarnos o recibimos error
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

    //Sacamos info del cliente con el que estamos hablando
    getnameinfo(&cliente, clienteLen,
                host, NI_MAXHOST,
                serv, NI_MAXSERV,
                NI_NUMERICHOST | NI_NUMERICSERV);
    std::cout << "Conexion desde : " << host <<" En el puerto: "<< serv << std::endl;

    bool bucle=true;
    while(bucle){

        //recepcion de alguien
        char buffer[80]; 
        int bytesRecibidos = recv(cliente_sock,buffer,79,0);
        buffer[bytesRecibidos] = '\0';

        //El cliente se ha desconectado
        if(bytesRecibidos <= 0){
            std::cout<<"Fin de la conexion"<<std::endl;
            break;
        }

        //Escribimos lo que nos acaba de llegar y lo reenviamos
        std::cout << buffer << std::endl;
        send(cliente_sock, buffer,bytesRecibidos,0);
    }


    //Terminamos
    std::cout <<"Cerramos el servidor "<< std::endl;
    close(sock);
    freeaddrinfo(resultado);
    return 0;
}
