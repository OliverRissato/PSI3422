//client1.cpp

#include "projeto.hpp"

int estado=0; //0=nao_apertado, 1=apertou_botao_1 2=apertou_botao_2
void on_mouse(int event, int c, int l, int flags, void* userdata) 
{ //Funcao callback
    if (event==EVENT_LBUTTONDOWN) 
    {
        if ( 160<=l && l<240 && 0<=c && c<80 ) estado=1;

        else if ( 160<=l && l<240 && 80<=c && c<160 ) estado=2;

        else if ( 160<=l && l<240 && 160<=c && c<240 ) estado=3;

        else if ( 80<=l && l<160 && 0<=c && c<80 ) estado=4;

        else if ( 80<=l && l<160 && 80<=c && c<160 ) estado=5;

        else if ( 80<=l && l<160 && 160<=c && c<240 ) estado=6;

        else if ( 0<=l && l<80 && 0<=c && c<80 ) estado=7;

        else if ( 0<=l && l<80 && 80<=c && c<160 ) estado=8;

        else if ( 0<=l && l<80 && 160<=c && c<240 ) estado=9;

        else estado=0;

    }

    else if (event==EVENT_LBUTTONUP) 
    {
        estado=0;
    }
}

int main(int argc, char *argv[])
{
    if (argc!=2) erro("client6 servidorIpAddr\n");
    CLIENT client(argv[1]);

    Mat_<COR> a;

    COR cinza(128,128,128);
    COR vermelho(0,0,255);
    Mat_<COR> gui(240,240,cinza);
    namedWindow("janela",WINDOW_NORMAL);
    resizeWindow("janela",240, 560);
    setMouseCallback("janela", on_mouse);

    while (true) 
    {
        gui.setTo(cinza);

        Mat_<COR> a;
        Mat_<COR> tudo;
        uint32_t ack = 0;

        client.receiveImgComp(a);

        if (a.total())
        {

            ack = 1;
        }

        client.sendUint(ack);

        if (estado==1) 
        {
            for (int l=160; l<240; l++)
                for (int c=0; c<80; c++)
                    gui(l,c)=vermelho;
        } 
        else if (estado==2) 
        {
            for (int l=160; l<240; l++)
                for (int c=80; c<160; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==3) 
        {
            for (int l=160; l<240; l++)
                for (int c=160; c<240; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==4) 
        {
            for (int l=80; l<160; l++)
                for (int c=0; c<80; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==5) 
        {
            for (int l=80; l<160; l++)
                for (int c=80; c<160; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==6) 
        {
            for (int l=80; l<160; l++)
                for (int c=160; c<240; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==7) 
        {
            for (int l=0; l<80; l++)
                for (int c=0; c<80; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==8) 
        {
            for (int l=0; l<80; l++)
                for (int c=80; c<160; c++)
                    gui(l,c)=vermelho;
        }
        else if (estado==9) 
        {
            for (int l=0; l<80; l++)
                for (int c=160; c<240; c++)
                    gui(l,c)=vermelho;
        }

        client.sendUint(estado);

        hconcat(gui, a, tudo);

        imshow("janela",tudo);

        int comando=(signed char)(waitKey(1));
        client.sendUint(comando);

        if (comando == 27)
        {
            printf("s"); 
            break;
        }
    }
}