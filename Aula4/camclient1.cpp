//camclient1.cpp

#include "projeto.hpp"

int main(int argc, char *argv[])
{
    if (argc!=2) erro("client6 servidorIpAddr\n");
    CLIENT client(argv[1]);

    Mat_<COR> a;
    namedWindow("janela");

    while (true) 
    {

        Mat_<COR> a;
        uint32_t ack = 0;

        client.receiveImg(a);

        if (a.total())
        {
            imshow("janela", a);

            ack = 1;
        }

        client.sendUint(ack);

        int comando=(signed char)(waitKey(1));
        client.sendUint(comando);

        if (comando == 27) break;
    }
}