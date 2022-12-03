//camserver2.cpp

#include "projeto.hpp"

int main(void)
{
    SERVER Server;
    Server.waitConnection();
    
    pwmInit();

    uint32_t comando;

    VideoCapture w(0);
    if (!w.isOpened()) erro("Erro: Abertura de webcam 0.");
    w.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    w.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    Mat_<COR>a;

    uint32_t ack = 1;

    while (true) 
    {
        
        uint32_t gui = 0;

        if(ack)
        {
            w >> a;

            Server.sendImgComp(a);
        }


        Server.receiveUint(ack);


        Server.receiveUint(gui);

        cout << gui << endl;

        executeCommand(gui);

        Server.receiveUint(comando);

        if (comando == 27)
        {
            printf("s \n");
            break;
        } 
    }
}