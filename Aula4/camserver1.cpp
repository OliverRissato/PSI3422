//camserver1.cpp

#include "projeto.hpp"

int main(void)
{
    SERVER Server;
    Server.waitConnection();
    
    uint comando;

    VideoCapture w(0);
    if (!w.isOpened()) erro("Erro: Abertura de webcam 0.");
    w.set(CV_CAP_PROP_FRAME_WIDTH, 480);
    w.set(CV_CAP_PROP_FRAME_HEIGHT, 640);
    Mat_<COR>a;

    uint32_t ack = 1;

    while (true) 
    {
        
        if(ack)
        {
            w >> a;

            Server.sendImg(a);
        }


        Server.receiveUint(ack);


        Server.receiveUint(comando);

        if (comando == 27) break;
    }
}