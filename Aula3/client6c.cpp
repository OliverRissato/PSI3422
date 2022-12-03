//client6c.cpp
#include "projeto.hpp"

int main(int argc, char *argv[]) 
{
    if (argc!=2) erro("client6b servidorIpAddr\n");
    CLIENT client(argv[1]);
    uint32_t u;
    client.receiveUint(u);
    cout << u << endl;
    client.sendUint(3333333333);
}