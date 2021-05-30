#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Preparamos las estructuras que usaremos para crear el socket
    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));
    hintsBusqueda.ai_family = AF_INET;  
    hintsBusqueda.ai_socktype = SOCK_DGRAM;

    //Sacamos la info de la direccion que se nos ha especificado
    int infoDir = getaddrinfo(address, port, &hintsBusqueda, &resultado);
    if(infoDir != 0)
    {
        std::cerr<<"Error al sacar la direccion del server solicitada\n";
        exit(-1);
    }

    //Creamos el socket que se ajuste a nuestra peticion
    sd=socket(resultado->ai_family, resultado->ai_socktype,0);
    if(sd == -1){
        std::cerr<<"Error al crear el socket solicitado\n";
        exit(-1); 
    }

    //Una vez hecho todo, nos quedamos con lo que nos interesa y limpiamos
    sa = *resultado->ai_addr;
    sa_len = resultado->ai_addrlen;
    freeaddrinfo(resultado);
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Se transforma el objeto serializable a un formato con el que se pueda trabajar
    //Se manda el objeto
    obj.to_bin();
    int bytesMandados = sendto(sd, obj.data(),obj.size(),0,&sock.sa,sock.sa_len);

    //Si ha habido error, informamos, si se ha mandado correctamente devolvemos 0
    if(bytesMandados == -1) return -1;
    else return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Se transforma el sockaddr a un formato con el que se pueda trabajar y se comprueba
    struct sockaddr_in * a = (struct sockaddr_in*) &s1.sa;
    struct sockaddr_in * b = (struct sockaddr_in*) &s2.sa;

    return a->sin_family == b->sin_family && 
           a->sin_port == b->sin_port && 
           a->sin_addr.s_addr == b->sin_addr.s_addr;
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

