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

    //Metemos el nombre
    int n = nick.length();
    char char_ComunicacionNick[n+1];
    strcpy(char_ComunicacionNick, nick.c_str());
    memcpy(tmp,char_ComunicacionNick, sizeof(char_ComunicacionNick));
    tmp = tmp+(sizeof(char_ComunicacionNick));

    //metemos el mensage
    n = message.length();
    char char_ComunicacionMensage[n+1];
    strcpy(char_ComunicacionMensage, message.c_str());
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
    int sizeNombre = strlen(tmp);  //Sacamos la longitud hasta un \0 pero sin contar este
    sizeNombre++;                   //Añadimos el retorno de carro
    nick.resize(sizeNombre);
    memcpy(&nick[0],tmp,sizeNombre);
    tmp = tmp+sizeof(sizeNombre);

    //Metemos el mensage
    int sizeMensage = strlen(tmp); //Sacamos la longitud hasta un \0 pero sin contar este
    sizeMensage++;                  //Añadimos el retorno de carro
    message.resize(sizeMensage);
    memcpy(&message[0],tmp,sizeMensage);
    tmp = tmp+sizeMensage;

    return 0;
}
    // std::string nombreLLega="";
    // while(*tmp  != '\0'){
    //     nombreLLega = nombreLLega + (*tmp);
    //     tmp++;
    // }
    // nombreLLega = nombreLLega + (*tmp);
    // tmp++;
    // nick = nombreLLega;



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

        clients.push_back(clients.move());
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
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
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

