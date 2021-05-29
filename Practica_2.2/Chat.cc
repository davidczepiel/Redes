#include "Chat.h"
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define MESSAGESIZE 80
#define NICKSIZE 8

void ChatMessage::to_bin()
{
    //Preparamos la zona donde almacenar los datos
    alloc_data(MESSAGE_SIZE);
    memset(_data, 0, MESSAGE_SIZE);
    char* tmp = _data;

    //Metemos el type
    memcpy(tmp,&type,sizeof(type));
    tmp = tmp+sizeof(type);

    //Metemos el nombre 
    memcpy(tmp,nick.c_str(), (sizeof(char)*8));
    tmp = tmp+(sizeof(char)*8);

    //metemos el mensage
    memcpy(tmp,message.c_str(),(sizeof(char)*80));

}

int ChatMessage::from_bin(char * bobj)
{
    //Preparamos los datos de donde vamos a leer
    alloc_data(MESSAGE_SIZE);
    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);
    char* tmp = _data;

    //Leemos el type
    memcpy(&type,tmp,sizeof(type));
    tmp = tmp+sizeof(type);

    //Leemos el nombre
    char char_ComunicacionNick[8] ;
    memcpy(char_ComunicacionNick,tmp,sizeof(char_ComunicacionNick));
    nick.append(char_ComunicacionNick);
    tmp = tmp+sizeof(char_ComunicacionNick);

    //Leemos el mensage
    char char_ComunicacionMensage[80] ;
    memcpy(char_ComunicacionMensage,tmp,sizeof(char_ComunicacionMensage));
    message.append(char_ComunicacionMensage);
    
    return 0;
}



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


void ChatServer::do_messages()
{
    while (true)
    {
        //Preparo el mensage que en un futuro se va a mandar y me hago con el socket que se esta comunicando conmigo
        ChatMessage receptor;
        Socket* nuevo = (Socket*)1;
        socket.recv(receptor,nuevo);

        switch(receptor.type){

            ///////////////////////////////////////////////
            ///Alguien se ha entrado de la sala
            ///////////////////////////////////////////////
            case ChatMessage::MessageType::LOGIN : {
                std::cout<<"CONEXION"<<std::endl;
                //Se pasa por el vector buscando la persona en cuestion
                auto iter = clients.begin();
                while(iter != clients.end()){
                    if(*iter->get() == *nuevo) break;
                    ++iter;
                }

                if( !(iter == clients.end()) ) 
                    std::cout<<"Alguien ya estaba en la sala ha intentado conectarse\n";
                else{
                    clients.push_back(std::unique_ptr<Socket>(std::move(nuevo)));
                    receptor.message = receptor.nick+" ha entrado a la sala";
                    std::cout<<receptor.nick<<" Se ha conectado"<<std::endl;
                    receptor.nick = "SERVER";
                    for(auto it = clients.begin(); it!=clients.end();++it)
                        socket.send(receptor,*it->get());
                }
                std::cout<<"Hay tantas personas conectadas: "<< clients.size()<<std::endl;

            }
            break;

            ///////////////////////////////////////////////
            ///Alguien se ha salido de la sala
            ///////////////////////////////////////////////
            case ChatMessage::MessageType::LOGOUT:{
                std::cout<<"DESCONEXION"<<std::endl;
                //Se pasa por el vector buscando la persona en cuestion
                auto iter = clients.begin();
                while(iter != clients.end()){
                    if(*iter->get() == *nuevo) break;
                    ++iter;
                }

                if(iter == clients.end()) 
                    std::cout<<"Alguien que no estaba conectado se desconecto\n";
                else{
                    clients.erase(iter);
                    receptor.message = receptor.nick+" ha salido de la sala";
                    receptor.nick = "SERVER";
                    for(auto it = clients.begin(); it!=clients.end();++it)
                        socket.send(receptor,*it->get());
                }
                std::cout<<"Hay "<< clients.size()<< " personas conectadas: "<< std::endl;
            }
            break;

            ///////////////////////////////////////////////
            ///Alguien se ha mandado algo
            ///////////////////////////////////////////////
            case ChatMessage::MessageType::MESSAGE:{
                std::cout<<"MENSAGE"<<std::endl;
                for(auto it = clients.begin(); it!=clients.end();++it)
                    if(!(*it->get() == *nuevo))  
                        socket.send(receptor,*it->get());                 
            }
            break;

            ///////////////////////////////////////////////
            ///Alguien se ha mandado algo que no se puede procesar
            ///////////////////////////////////////////////      
            default:
                std::cout<<"Se me ha mandado un tipo de mensage que no se procesar"<<std::endl;
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    //Mandamos un mensaje de que nos hemos ido
    std::string msg;
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;
    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        //Leemos de consola 
        std::string contenido;
        std::getline(std::cin,contenido);

        //Si hay que desconectarse terminamos
        if(contenido == "q"){
            std::cout<<"NOS DESCONECTAMOS"<<std::endl;
            logout();
            break;
        }

        //Mandamos el mensage
        ChatMessage men(nick, contenido);
        men.type = ChatMessage::MESSAGE;
        socket.send(men,socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibimos un mensage desde el servidor
        ChatMessage receptor;
        socket.recv(receptor);

        //Mostramos autor y contenido
        std::cout<<receptor.nick<<": "<<receptor.message<<std::endl;
    }
}

