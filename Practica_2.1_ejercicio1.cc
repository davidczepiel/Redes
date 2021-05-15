#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

#include <iostream>


int main(int argc, char **argv){

    //Error por si se ejecuta el programa de manera no adecuada
    if(argc <2){
        std::cout<<"El programa debe recibir una direccion de host"<<std::endl;
        return -1;
    }

    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    //Configuracion para sacar direcciones de todas las familias posibles
    //y de cualquier tipo de puerto
    hintsBusqueda.ai_family = AF_UNSPEC;  
    hintsBusqueda.ai_socktype = NULL;

    //Sacamos toda la informacion posible en resultado
    //O recibimos error y paramos
    int re = getaddrinfo(argv[1], NULL, &hintsBusqueda, &resultado);
    if(re !=0){
        std::cerr<<"Error de getaddrinfo por: "<<gai_strerror(re)<<std::endl;
        return 1;
    }

    std::cout<<"Direccion/Familia/TipoDePuerto"<<std::endl;
    for(struct addrinfo* i = resultado ; i != NULL; i = i->ai_next){
        //Buffers para almacenar la informacion
        char buff1[NI_MAXHOST];
        char buff2[NI_MAXSERV];
        getnameinfo(i->ai_addr, i->ai_addrlen,
                       buff1, NI_MAXHOST,
                       NULL, NI_MAXSERV, //Se especifica NULL porque no interesa que trate este campo
                       NI_NUMERICHOST);
        std::cout << buff1 << "    " << i->ai_family << "   " << i->ai_socktype << std::endl;
    }

    freeaddrinfo(resultado);
    return 0;
}