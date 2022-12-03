//client8.cpp
//testa sendVb e receiveVb
#include "projeto.hpp"

int main(int argc, char *argv[]) 
{
    if (argc!=2) erro("client6 servidorIpAddr\n");
    CLIENT client(argv[1]);
    vector<BYTE> vb;
    
    client.receiveVb(vb);
    if (testaVb(vb,111)) printf("Recebi corretamente %lu bytes %u\n",vb.size(),111);
    else printf("Erro na recepcao de %lu bytes %u\n",vb.size(),111);
    
    vb.assign(100000,222);
    client.sendVb(vb);
    
    client.receiveVb(vb);
    if (testaVb(vb,1)) printf("Recebi corretamente %lu bytes %u\n",vb.size(),1);
    else printf("Erro na recepcao de %lu bytes %u\n",vb.size(),1);
}