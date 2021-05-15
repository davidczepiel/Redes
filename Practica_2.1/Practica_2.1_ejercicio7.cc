#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <thread>
#include <mutex>

#include <iostream>


class ThreadTrabajador
{
public:
    ThreadTrabajador(int s){
        cliente_sock=s;
    }
 
    void TratarCliente()
    {
        bool bucle=true;
        while(bucle){
            //recepcion del cliente
            char buffer[80]; 
            int bytesRecibidos = recv(cliente_sock,buffer,79,0);
            buffer[bytesRecibidos] = '\0';

            //Se ha cerrado la conexion
            if(bytesRecibidos <= 0){
                std::cout<<"["<< std::this_thread::get_id() <<"] " <<"Fin de la conexion"<<std::endl;
                break;
            }

            //Mostramos lo recibido y reenviamos
            std::cout<<"["<< std::this_thread::get_id() <<"] Recibido:"  << buffer << std::endl;
            send(cliente_sock, buffer,bytesRecibidos,0);
        }

        close(cliente_sock);
        std::cout <<"["<< std::this_thread::get_id() <<"] " <<"Cerramos conexion con un cliente "<< std::endl;
    }
    private:
    int cliente_sock;
};


int main(int argc, char **argv){

    //Error por si se ejecuta el programa de manera no adecuada
    if(argc !=3){
        std::cout<<"El programa debe recibir una direccion IP y un puerto"<<std::endl;
        return -1;
    }

    //Info del server
    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));
    hintsBusqueda.ai_family = AF_INET; //Solo IPV4 que es con lo que trabajamos
    hintsBusqueda.ai_socktype = SOCK_STREAM; //UDP

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

    while(true){
        //Obtenemos un cliente
        int cliente_sock = accept(sock, &cliente, &clienteLen);
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        //Sacamos su informacion
        getnameinfo(&cliente, clienteLen,
                    host, NI_MAXHOST,
                    serv, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "Conexion desde : " << host <<" En el puerto: "<< serv << std::endl;
        
        //Creamos un thread que se haga cargo del cliente
        std::thread([cliente_sock](){
            ThreadTrabajador mt = ThreadTrabajador(cliente_sock);
            mt.TratarCliente();
        }).detach();
    }

    //Terminamos
    std::cout<<"Server cerrado por completo"<<std::endl;
    freeaddrinfo(resultado);
    close(sock);
    return 0;
}
