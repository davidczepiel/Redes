#include <thread>
#include "Chat.h"
#include <iostream>

int main(int argc, char **argv)
{
    ChatClient ec(argv[1], argv[2], argv[3]);

    std::thread net_thread([&ec](){ ec.net_thread(); });
    ec.login();

    net_thread.detach();
    ec.input_thread();
    
    return 0;
}

