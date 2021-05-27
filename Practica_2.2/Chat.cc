#include "Chat.h"
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define MESSAGESIZE 80
#define NICKSIZE 8

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);
    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;

    //Metemos el type
    memcpy(tmp,&type,sizeof(type));
    tmp = tmp+sizeof(type);

    //Metemos el nombre (este solo puede tener hasta 8 caracteres)
    char char_ComunicacionNick[8] = NULL;
    strncpy(char_ComunicacionNick, nick.c_str(),8);
    char_ComunicacionNick[7] = '\0';
    memcpy(tmp,char_ComunicacionNick, sizeof(char_ComunicacionNick));
    tmp = tmp+(sizeof(char_ComunicacionNick));

    //metemos el mensage
    char char_ComunicacionMensage[80] = NULL;
    strncpy(char_ComunicacionMensage, message.c_str(),80);
    char_ComunicacionMensage[79]= '\0';
    memcpy(tmp,char_ComunicacionMensage,sizeof(char_ComunicacionMensage));

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);
    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);
  
    char* tmp = data;
    //Metemos el type
    memcpy(&type,tmp,sizeof(type));
    tmp = tmp+sizeof(type);

    //Metemos el nombre
    char char_ComunicacionNick[8] = NULL;
    memcpy(char_ComunicacionNick,tmp,sizeof(char_ComunicacionNick));
    nick.append(char_ComunicacionNick);
    tmp = tmp+sizeof(char_ComunicacionNick);

    //Metemos el mensage
    char char_ComunicacionMensage[80] = NULL;
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
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage receptor;
        Socket* nuevo ;
        socket.recv(receptor,nuevo);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        switch(receptor.type){
            case MessageType::LOGIN:
                if(!alreadyConnected(nuevo)){
                    clients.push_back(std::nuevo.move());
                }
            break;

            case MessageType::LOGOUT:
                if(removeUser(nuevo)){
                    std::cout<<"Alguien que no estaba conectado se desconecto\n";
                }
            break;

            case MessageType::MESSAGE:
                broadcast(receptor,nuevo);
            break;
        }
    }
}

void ChatServer::broadcast(ChatMessage men,Socket* emisor)
{
    for(int i=0;i<clients.size();i++){
        socket.send(men,clients[i].get());
    }
}


bool ChatServer::removeUser(Socket* user)
{
    for(auto it= clients.begin();i != clients.end();i++){
        if((*it).get() == user){
            clientes.erase(it);
            return true;
        }
    }
    return false;
}

bool ChatServer::alreadyConnected(Socket* user)
{
    for(auto it= clients.begin();i != clients.end();i++){
        if((*it).get() == user){
            return true;
        }
    }
    return false;
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
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string contenido;
        std::getline(std::cin,contenido);
        ChatMessage men(nick, contenido);
        men.type = ChatMessage::MESSAGE;
        socket.send(men,socket);

    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage receptor;
        socket.recv(receptor,nullptr);
        std::cout<<socket<<" nick: "<<receptor.nick<<" "<<receptor.message<<std::endl;
    }
}

