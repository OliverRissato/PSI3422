#include "projeto.hpp"
#include "templateMatching.cpp"

int main(int argc, char *argv[]) {
    if (argc<2) erro("camclient1 servidorIpAddr\n");

    CLIENT client(argv[1]);
    namedWindow("janela",1);
    Mat_<COR> img;
    char ch = -1;
    string arquivo = "";

    COR cinza(128,128,128);
    COR vermelho(0,0,255);

    namedWindow("janela",WINDOW_NORMAL);
    
    if (argc == 3)
        arquivo = argv[2];
    VideoWriter vo(arquivo,
    CV_FOURCC('X','V','I','D'),
    16,
    Size(320,240));
    while (ch != 27)
    {
        int x = 0;
        int escalaCard = 0;
        int card = 0;
        // confirma recebimento e espera novo frame
        client.sendUint(100);
        client.receiveImgComp(img);
        // faz o template matching e obtem se ha um cartao, sua posicao e escala
        templateMatching(img, &x, &card, &escalaCard);
        imshow("janela",img);
        if (argc == 3){
            vo << img;
        }
        // envia dados do cartao
        client.sendUint(card);
        client.sendUint(x);
        client.sendUint(escalaCard);

        ch=(signed char)waitKey(30);
        // caso ESC
        if (ch == 27){
            client.sendUint(0);
            break;
        }
    }
    return 0;
}
