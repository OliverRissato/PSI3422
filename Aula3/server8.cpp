//server8.cpp
//testa sendVb e receiveVb
#include "projeto.hpp"

int main(void) 
{
    SERVER server;
    server.waitConnection();
    vector<BYTE> vb;
    
    vb.assign(100000,111);
    server.sendVb(vb);
    
    server.receiveVb(vb);
    if (testaVb(vb,222)) printf("Recebi corretamente %lu bytes %u\n",vb.size(),222);
    else printf("Erro na recepcao de %lu bytes %u\n",vb.size(),222);
    
    vb.assign(100000,2);
    server.sendVb(vb);
}