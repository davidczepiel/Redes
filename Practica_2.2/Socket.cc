#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    struct addrinfo hintsBusqueda;
    struct addrinfo * resultado;
    memset((void *)&hintsBusqueda,0,sizeof(struct addrinfo));

    hintsBusqueda.ai_family = AF_INET;  
    hintsBusqueda.ai_socktype = SOCK_DGRAM;

    getaddrinfo(address, port, &hintsBusqueda, &resultado);
    sd=socket(AF_INET,SOCK_DGRAM,0);

    sa = resultado->ai_addr;
    sa_len = resultado->ai_addrlen;
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
    //Serializar el objeto
    //Enviar el objeto binario a sock usando el socket sd
    obj.to_bin();
    sendto(sd, obj.data(),obj.size(),0,sock.sa,sock.sa_len);
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere
    return s1.sin_family == s2.sin_family && 
            s1.sin_addr.s_addr == s2.sin_addr.s_addr && 
            s1.sin_port == s2.sin_port;
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

