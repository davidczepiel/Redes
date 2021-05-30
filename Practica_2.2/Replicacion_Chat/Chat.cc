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
        Socket* nuevo = (Socket*)&receptor;    //Hago que el puntero apunte a cualquier cosa para que el recv me cree un nuevo socket
        socket.recv(receptor,nuevo);

        switch(receptor.type){

            ///////////////////////////////////////////////
            ///Alguien ha entrado de la sala
            ///////////////////////////////////////////////
            case ChatMessage::MessageType::LOGIN : {
                std::cout<<"CONEXION"<<std::endl;
                //Se pasa por el vector buscando la persona en cuestion
                auto iter = clients.begin();
                while(iter != clients.end()){
                    if(*iter->get() == *nuevo) break;
                    ++iter;
                }
                //En caso de que esa persona ya estuviera conectada a la sala
                if( !(iter == clients.end()) ) 
                    std::cout<<"Alguien que ya estaba en la sala ha intentado reconectarse\n";
                //En caso de que no estuviera conectada
                else{
                    //Avisamos a todos los clientes de esto (esto lo informa el server)
                    clients.push_back(std::unique_ptr<Socket>(std::move(nuevo)));
                    receptor.message = receptor.nick+" ha entrado a la sala";
                    receptor.nick = "SERVER";
                    for(auto it = clients.begin(); it!=clients.end();++it)
                        socket.send(receptor,*it->get());
                }
                std::cout<<"Hay "<< clients.size()<< " personas conectadas"<< std::endl;

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

                //En caso de que no se halla encontrado a quien se quiere desconectar
                if(iter == clients.end()) 
                    std::cout<<"Alguien que no estaba conectado se desconecto\n";
                //Eliminamos a quien se quiere desconectar
                else{
                    //Avisamos a los clientes de esto
                    clients.erase(iter);
                    receptor.message = receptor.nick+" ha salido de la sala";
                    receptor.nick = "SERVER";
                    for(auto it = clients.begin(); it!=clients.end();++it)
                        socket.send(receptor,*it->get());
                }
                std::cout<<"Hay "<< clients.size()<< " personas conectadas"<< std::endl;
            }
            break;

            ///////////////////////////////////////////////
            ///Alguien ha mandado algo
            ///////////////////////////////////////////////
            case ChatMessage::MessageType::MESSAGE:{
                //Mandamos el mensage en cuestion a todos los conectados a la sala
                std::cout<<"MENSAGE"<<std::endl;
                for(auto it = clients.begin(); it!=clients.end();++it)
                    //Mandamos a todos los usuarios menos a aquel que ha mandado el mensage
                    if(!(*it->get() == *nuevo))  
                        socket.send(receptor,*it->get());                 
            }
            break;

            ///////////////////////////////////////////////
            ///Alguien ha mandado algo que no se puede procesar
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

