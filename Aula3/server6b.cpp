//server6b.cpp
//testa sendBytes e receiveBytes
#include "projeto.hpp"

int main(void) {
    SERVER server;
    server.waitConnection();

    const int n=100000;
    BYTE buf[n];
    memset(buf,111,n); //insere 111 em n bytes a partir do endereço buf
    server.sendBytes(n,buf);

    server.receiveBytes(n,buf);
    if (testaBytes(buf,214,n)) printf("Recebeu corretamente %d bytes %d\n",n,214);
    else printf("Erro na recepcao de %d bytes %d\n",n,214);
    
    memset(buf,111,n);
    server.sendBytes(n,buf);
}