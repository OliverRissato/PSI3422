//client1.cpp

#include "projetoC.hpp"

#define RAZAO 0.8663

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

    Mat_<FLT> T ;
    Mat_<FLT> TRezise[10];
    le(T,"quadrado.png");
    float escala[10];
    escala[0] = 0.1721;
    int iCard, jCard;
    bool card = false;

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
        Mat_<FLT> b;
        Mat_<FLT> RCC[10];
        Mat_<COR> tudo;
        uint32_t ack = 0;

        client.receiveImgComp(a);

        if (a.total())
        {

            ack = 1;
        }

        client.sendUint(ack);

        converte(a,b);
        for (int i = 0; i < 10; i++){
            if (i > 0)
                escala[i] = escala[i-1] * RAZAO;
            resize(T, TRezise[i], Size(), escala[i], escala[i], INTER_NEAREST);
            TRezise[i] = somaAbsDois(dcReject(TRezise[i],1.0));
            RCC[i]=matchTemplateSame(b,TRezise[i],CV_TM_CCORR,0.0);
        }
        float menor = 0;
        float pixel[20][5];
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                pixel[i][j] = 0;
            }
        }
        int count = 0;
        for (int k = 0; k < 10; k++)
        {
            for (int i = 0; i < 240; i++)
            {
                for (int j = 0; j < 320; j++)
                {
                    if (RCC[k][i][j] > menor){
                        for (int l = 0; l < 20; l++)
                        {
                            int nEscala = pixel[l][3];

                            if ((pixel[l][1]+40 > i && pixel[l][1]-40 < i)||(pixel[l][2]+40> j && pixel[l][2]-40< j)){
                                if (pixel[l][0] <  RCC[k][i][j]){
                                    pixel[l][0] = RCC[k][i][j];
                                    pixel[l][1] = i;
                                    pixel[l][2] = j;
                                    pixel[l][3] = k;
                                    break;
                                }
                            }
                            else if (pixel[l][0] == menor){
                                count++;
                                if (count > 20){
                                    menor = RCC[k][i][j];
                                }
                                pixel[l][0] = RCC[k][i][j];
                                pixel[l][1] = i;
                                pixel[l][2] = j;
                                pixel[l][3] = k;
                                break;
                            }
                        }
                        for (int l = 0; l < 20; l++)
                        {
                            if (pixel[l][0] < menor){
                                menor = pixel[l][0];
                            }
                        }
                        
                    }
                }
            }
        }
        
        Mat_<FLT> RNCC[10];
        bool escalasCC[10];
        for (int i = 0; i < 10; i++)
        {
            escalasCC[i] = false;
        }
        
        int thickness = 2;
        for (int l = 0; l < 20; l++)
        {
            int i = pixel[l][1];
            int j = pixel[l][2];
            int k = pixel[l][3];
            if(escalasCC[k] == false){
                RNCC[k]=matchTemplateSame(b,TRezise[k],CV_TM_CCOEFF_NORMED,0.0);
                escalasCC[k] == true;
            }

            pixel[l][4] = RNCC[k][i][j];
            line(a, Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
            line(a, Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
            line(a, Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
            line(a, Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(128,128,0),thickness);
            putText(a, std::to_string((int)pixel[l][3]), Point(j + (int)(escala[k]*401)/2+10, i-15), 0, 0.5, Scalar(128,128,0), 0.5);
            putText(a, std::to_string((int)(pixel[l][0]*100)), Point(j + (int)(escala[k]*401)/2+10, i), 0, 0.5, Scalar(128,128,0), 0.5);
            putText(a, std::to_string((int)(pixel[l][4]*100)), Point(j + (int)(escala[k]*401)/2+10, i+15), 0, 0.5, Scalar(128,128,0), 0.5);
        
        }
        for (int l = 0; l < 20; l++)
        {
            int i = pixel[l][1];
            int j = pixel[l][2];
            int k = pixel[l][3];
            if (RNCC[k][i][j] > 0.55 && card == false){
                card == true;
                iCard = i;
                jCard = j;
                line(a, Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),6);
                line(a, Point(j + (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),6);
                line(a, Point(j + (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Scalar(0,128,128),6);
                line(a, Point(j - (int)(escala[k]*401)/2,i + (int)(escala[k]*401)/2), Point(j - (int)(escala[k]*401)/2,i - (int)(escala[k]*401)/2), Scalar(0,128,128),6);
                putText(a, std::to_string((int)pixel[l][3]), Point(j + (int)(escala[k]*401)/2+10, i-15), 0, 0.5, Scalar(0,128,128), 0.5);
                putText(a, std::to_string((int)(pixel[l][0]*100)), Point(j + (int)(escala[k]*401)/2+10, i), 0, 0.5, Scalar(0,128,128), 0.5);
                putText(a, std::to_string((int)(pixel[l][4]*100)), Point(j + (int)(escala[k]*401)/2+10, i+15), 0, 0.5, Scalar(0,128,128), 0.5);
            }
        }
        

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