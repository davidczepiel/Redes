#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <thread>

#include <iostream>

#define MAXTHREADS  5


class MessageThread
{
public:
    MessageThread(int s,int i){
        sock=s;
    }
    
    void do_message()
    {
        while(true){
                //Preparacion, máquinas que se comunican
            char buffer[80]; 
            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            //Socket por el que hay comunicación
            struct sockaddr cliente;
            socklen_t  clienteLen = sizeof(cliente);

            //recepcion de alguien
            int bytesRecibidos = recvfrom(sock,buffer,80,0,&cliente,&clienteLen);
            buffer[bytesRecibidos] = '\0';
            if(bytesRecibidos == -1){
                std::cerr<<"["<< std::this_thread::get_id() <<"] " << "Error al recibir datos de un cliente"<<std::endl;
                return ;
            }
            getnameinfo(&cliente, clienteLen,
                        host, NI_MAXHOST,
                        serv, NI_MAXSERV,
                        NI_NUMERICHOST | NI_NUMERICSERV);

            
            std::cout<<"["<< std::this_thread::get_id() <<"] " << "Me han hablado desde : " << host <<" En el puerto: "<< serv << std::endl;
            sleep(5);
            switch (buffer[0]){
                    //Me han pedido la hora
                    case 't':{
                        time_t tiempo= time(NULL);
                        struct tm *hora = localtime(&tiempo);
                        size_t tamHora=strftime(buffer,100,"%R",hora);
                        sendto(sock, buffer,tamHora,0,&cliente,clienteLen);
                        std::cout<<"["<< std::this_thread::get_id() <<"] " << "Mando la fecha a "<< host <<" "<< serv << std::endl;
                        }
                    break;
                    //Me han pedido la fecha
                    case 'd':{
                        time_t tiempo= time(NULL);
                        struct tm *fecha = localtime(&tiempo);
                        size_t tamFecha=strftime(buffer,100,"%D",fecha);
                        sendto(sock, buffer,tamFecha,0,&cliente,clienteLen);
                        std::cout<<"["<< std::this_thread::get_id() <<"] " << "Mando la hora a "<< host <<" "<< serv <<  std::endl;
                        }
                    break;

                    //Me han pedido cerrar el sever
                    case 'q':{
                        char cerrar[] = "Cierre del servidor\0";
                        sendto(sock, cerrar,sizeof(cerrar),0,&cliente,clienteLen);
                        }
                    break;

                    //Me han pedido algo que no se
                    default:
                        std::cout <<"["<< std::this_thread::get_id() <<"] " << "Comando no soportado "<< buffer << std::endl;
                        char noSoportado[] = "Comando no soportado\0";
                        sendto(sock, noSoportado,sizeof(noSoportado),0,&cliente,clienteLen);
                    break;
                }       
            }
    }
    
private:
  int sock;
};

int main(int argc, char **argv){

    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;

    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    hintsBusqueda.ai_family = AF_INET; //Solo IPV4 que es con lo que trabajamos
    hintsBusqueda.ai_socktype = SOCK_DGRAM; //UDP

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


    // std::thread pool[MAXTHREADS];
    // for(int i=0;i<MAXTHREADS;i++){
    //     pool[i] = std::thread(hazMensaje,sock,i);
    // }

    //Creamos los threads que se van a encargar de ir procesando mensajes constantemente
    std::thread pool[MAXTHREADS];
    for(int i=0;i<MAXTHREADS;i++){
        MessageThread *mt = new MessageThread(sock,i);
        std::thread([&mt](){
            mt->do_message();
            delete mt;
        }).detach();
    }


    //El thread principal se queda bloqueado hasta que le demos a la q
    //en cuyo caso cierra el socket (con lo que el resto de threads terminan su trabajo)
    //Y termina su ejecución, por lo que todos los threads terminan
    std::string lectura="";
    while(lectura != "q"){
        std::cin>>lectura;
    }
    std::cout <<"Cerramos el servidor "<< std::endl;
    close(sock);
    freeaddrinfo(resultado);

    return 0;
}
