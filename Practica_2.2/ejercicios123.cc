#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //
        int size = (80*sizeof(char)) + (2* sizeof(x));
        alloc_data(size);
        char* tmp= _data;

        memcpy(tmp,name, 80*sizeof(char));
        tmp = tmp+80*sizeof(char);
        memcpy(tmp,&x,sizeof(x));
        tmp = tmp+sizeof(x);
        memcpy(tmp,&y,sizeof(y));

    }

    int from_bin(char * data)
    {
        char* tmp = data;
        memcpy(name,tmp, 80*sizeof(char));
        tmp = tmp+80*sizeof(char);
        memcpy(&x,tmp,sizeof(x));
        tmp = tmp+sizeof(x);
        memcpy(&y,tmp,sizeof(y));
        return 0;
    }

    int getX(){
        return x;
    }

    int getY(){
        return y;
    }
    char* getName(){
        return name;
    }
    void prueba(){
        strncpy(name, "Hola que tal estas\0", 80);
    }

public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("HolaBuenas", 123, 987);

    int id =0;
    // 1. Serializar el objeto one_w
    id = open("./dataJugador",O_CREAT |  O_TRUNC | O_RDWR , 6666);
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    write(id,one_w.data(),one_w.size());
    close(id);   

    one_w.prueba();
    one_w.from_bin(one_w.data());
    std::cout<<"Nombre= "<<one_w.getName()<<" X= "<<one_w.getX()<<" Y= "<<one_w.getY()<< std::endl;

    // 3. Leer el fichero
    char* lector = (char*)malloc(one_w.size());
    id = open("./dataJugador",O_RDONLY);
    read(id,lector,one_w.size());

    // 4. "Deserializar" en one_r
    one_r.from_bin(lector);

    // 5. Mostrar el contenido de one_r
    std::cout<<"Nombre= "<<one_r.getName()<<" X= "<<one_r.getX()<<" Y= "<<one_r.getY()<< std::endl;
    free(lector);

    return 0;
}

