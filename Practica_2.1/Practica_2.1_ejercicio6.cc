#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <thread>
#include <iostream>

#define MAXTHREADS  5

class ThreadTrabajador
{
public:
    ThreadTrabajador(int s){
        sock=s;
    }
    
    void trataMensaje()
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
            int bytesRecibidos = recvfrom(sock,buffer,79,0,&cliente,&clienteLen);
            buffer[bytesRecibidos] = '\0';
            if(bytesRecibidos == -1){
                std::cerr<<"["<< std::this_thread::get_id() <<"] " << "Error al recibir datos de un cliente"<<std::endl;
                return ;
            }

            //Info del cliente
            getnameinfo(&cliente, clienteLen,
                        host, NI_MAXHOST,
                        serv, NI_MAXSERV,
                        NI_NUMERICHOST | NI_NUMERICSERV);
            std::cout<<"["<< std::this_thread::get_id() <<"] " << "Me han hablado desde : " << host <<" En el puerto: "<< serv << std::endl;
            
            //Simulamos que estamos procesando el mensaje
            sleep(5);
            
            //procesamos
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
                    char cerrar[] = "Cerramos conexion con un cliente\0";
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


    //Error por si se ejecuta el programa de manera no adecuada
    if(argc !=3){
        std::cout<<"El programa debe recibir una direccion IP y un puerto"<<std::endl;
        return -1;
    }

    //Sacamos la info del server
    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));
    hintsBusqueda.ai_family = AF_INET; //Solo IPV4 
    hintsBusqueda.ai_socktype = SOCK_DGRAM; //UDP

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

    //Creamos los threads que se van a encargar de ir procesando mensajes constantemente
    std::thread pool[MAXTHREADS];
    for(int i=0;i<MAXTHREADS;i++){
        ThreadTrabajador *mt = new ThreadTrabajador(sock);
        std::thread([&mt](){
            mt->trataMensaje();
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

    //Terminamos
    std::cout <<"Cerramos el servidor "<< std::endl;
    close(sock);
    freeaddrinfo(resultado);

    return 0;
}
