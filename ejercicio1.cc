#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

#include <iostream>


int main(int argc, char **argv){

    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;

    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    hintsBusqueda.ai_family = AF_INET;
    hintsBusqueda.ai_socktype = SOCK_STREAM;

    int re = getaddrinfo(argv[1], argv[2], &hintsBusqueda, &resultado);

    if(re !=0){
        std::cerr<<"Error de getaddrinfo por: "<<gai_strerror(re)<<std::endl;
        return 1;
    }

    for(struct addrinfo* i = resultado ; i != NULL; i = i->ai_next){
        char buff1[NI_MAXHOST];
        char buff2[NI_MAXSERV];
        getnameinfo(i->ai_addr, i->ai_addrlen,
                       buff1, NI_MAXHOST,
                       buff2, NI_MAXSERV,
                       NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout<<"Host: " << buff1 <<" Port: "<<buff2<<std::endl;
    }
    freeaddrinfo(resultado);

    return 0;
}