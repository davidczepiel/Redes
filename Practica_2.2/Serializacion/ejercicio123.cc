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
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //Preparamos el contenedor de nuestros datos
        int size = (MAX_NAME*sizeof(char)) + (2* sizeof(x));
        alloc_data(size);
        char* tmp= _data;

        //Metemos el nombre
        memcpy(tmp,name, MAX_NAME*sizeof(char));
        tmp = tmp+(MAX_NAME*sizeof(char));
        //La X
        memcpy(tmp,&x,sizeof(x));
        tmp = tmp+sizeof(x);
        //La Y
        memcpy(tmp,&y,sizeof(y));

    }

    int from_bin(char * data)
    {
        //Tomamos los datos y leemos el nombre
        char* tmp = data;
        memcpy(name,tmp, MAX_NAME*sizeof(char));
        tmp = tmp+(MAX_NAME*sizeof(char));
        //La X
        memcpy(&x,tmp,sizeof(x));
        tmp = tmp+sizeof(x);
        //La Y
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

public:
    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Jugador1", 123, 987);

    int id =0;
    // 1. Serializar el objeto one_w
    id = open("./dataJugador",O_CREAT |  O_TRUNC | O_RDWR , 00666);
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    write(id,one_w.data(),one_w.size());
    close(id);   

    std::cout<<"Soy el lector antes de leer\n Nombre= "<<one_r.getName()<<" X= "<<one_r.getX()<<" Y= "<<one_r.getY()<< std::endl;

    // 3. Leer el fichero
    char* lector = (char*)malloc(one_w.size());
    id = open("./dataJugador",O_RDONLY);
    read(id,lector,one_w.size());

    // 4. "Deserializar" en one_r
    one_r.from_bin(lector);

    // 5. Mostrar el contenido de one_r
    std::cout<<"Soy el lector despues de leer\n Nombre= "<<one_r.getName()<<" X= "<<one_r.getX()<<" Y= "<<one_r.getY()<< std::endl;
    free(lector);
    return 0;
}

