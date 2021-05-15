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

    //Configuramos direcciones IPV4 y UDP
    hintsBusqueda.ai_family = AF_INET; 
    hintsBusqueda.ai_socktype = SOCK_DGRAM; 

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

    bool bucle=true;
    while(bucle){
        //Preparacion, máquinas que se comunican
        char buffer[80]; 
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        //Socket por el que hay comunicación
        struct sockaddr cliente;
        socklen_t  clienteLen = sizeof(cliente);

        //recepcion de alguien
        int bytesRecibidos = recvfrom(sock,buffer,79,0,&cliente,&clienteLen);
        buffer[bytesRecibidos] = '\0';
        if(bytesRecibidos == -1){
            std::cerr<<"Error al recibir datos de un cliente"<<std::endl;
            return -1;
        }

        //Sacamos info del cliente que nos esta hablando
        getnameinfo(&cliente, clienteLen,
                       host, NI_MAXHOST,
                       serv, NI_MAXSERV,
                       NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "Me han hablado desde : " << host <<" En el puerto: "<< serv << std::endl;
        
        //Procesamos
        switch (buffer[0]){
            //Me han pedido la hora
            case 't':{
                time_t tiempo= time(NULL);
                struct tm *hora = localtime(&tiempo);
                size_t tamHora=strftime(buffer,100,"%R",hora);
                sendto(sock, buffer,tamHora,0,&cliente,clienteLen);
                }
            break;
            //Me han pedido la fecha
            case 'd':{
                time_t tiempo= time(NULL);
                struct tm *fecha = localtime(&tiempo);
                size_t tamFecha=strftime(buffer,100,"%D",fecha);
                sendto(sock, buffer,tamFecha,0,&cliente,clienteLen);
                }
            break;

            //Me han pedido cerrar el sever
            case 'q':{
                bucle = false;
                char cerrar[] = "Cierre del servidor\0";
                sendto(sock, cerrar,sizeof(cerrar),0,&cliente,clienteLen);
                }
            break;

            //Me han pedido algo que no se procesar
            default:
                std::cout <<"Comando no soportado "<< buffer << std::endl;
                char noSoportado[] = "Comando no soportado\0";
                sendto(sock, noSoportado,sizeof(noSoportado),0,&cliente,clienteLen);
            break;
        }        
    }


    //Limpieza de ejecucion
    std::cout <<"Cerramos el servidor "<< std::endl;
    close(sock);
    freeaddrinfo(resultado);

    return 0;
}
