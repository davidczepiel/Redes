#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

#include <iostream>


int main(int argc, char **argv){

    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;

    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    //Solicitar toda clase de direcciones 
    hintsBusqueda.ai_family = AF_UNSPEC;  
    //No especificar puertos, se consiguen todos los puertos posibles (de todos los tipos)
    hintsBusqueda.ai_socktype = NULL;

    int re = getaddrinfo(argv[1], NULL, &hintsBusqueda, &resultado);

    if(re !=0){
        std::cerr<<"Error de getaddrinfo por: "<<gai_strerror(re)<<std::endl;
        return 1;
    }

    std::cout<<"Direccion/Familia/TipoDePuerto"<<std::endl;
    for(struct addrinfo* i = resultado ; i != NULL; i = i->ai_next){
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