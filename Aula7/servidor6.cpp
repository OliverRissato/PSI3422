// servidor6.cpp

#include "projeto.hpp"
#include <cekeikon.h>
#include <wiringPi.h>
#include <softPwm.h>
#define HALFSPEED 96
#define SPEED  90
#define SPEEDL 60
#define SPEEDR 84
#define KX 1.5
#define TIMERETURN 1.7

void motores(int x, bool card, int escalacard);

int main() {
    SERVER server;
    wiringPiSetup();
    if (softPwmCreate(1, 0, 100)) erro("erro PWM1");
    if (softPwmCreate(0, 0, 100)) erro("erro PWM0");
    if (softPwmCreate(2, 0, 100)) erro("erro PWM2");
    if (softPwmCreate(3, 0, 100)) erro("erro PWM3");
    server.waitConnection();
    Mat_<COR> img;
    Mat_<COR> img_send;
    uint32_t a;

    VideoCapture cam(0);
    if (!cam.isOpened()) erro("Erro: Abertura de webcam 0.");
    cam.set(CV_CAP_PROP_FRAME_WIDTH,320);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT,240);

    server.receiveUint(a);

    int estado = 0;

    TimePoint t1, t2;
    double t;
    uint32_t digitoLido;


    while (a == 100){
        uint32_t card = 0;
        uint32_t escalaCard = 0;
        uint32_t x = 0;
        uint32_t digito = 0;
        // obtencao do quadro atual
        cam.grab();
        cam.grab();
        cam >> img;
        img.convertTo(img_send, -1, 1, 80); //increase the brightness by 80
        server.sendImgComp(img_send);
        // recebimento dos dados do cartao
        server.receiveUint(card);
        server.receiveUint(x);
        server.receiveUint(escalaCard);
        server.receiveUint(digito);
        // envio do estado atual
        server.sendUint(estado);
        server.receiveUint(a);
        
        // maquina de estados
        switch (estado)
        {
        // segue o cartao
        case 0:
            softPwmWrite(0, 100); softPwmWrite(1, 100);
            softPwmWrite(2, 100); softPwmWrite(3, 100);
            if (escalaCard >= 2)
                motores(x, card, escalaCard);
            else if (card == 1 && escalaCard < 2){
                estado = 1;
                t1=timePoint();
                softPwmWrite(0, 100); softPwmWrite(1, 100);
                softPwmWrite(2, 100); softPwmWrite(3, 100);
            }
            break;
        // parada
        case 1:
            t2=timePoint();
            t=timeSpan(t1,t2);
            softPwmWrite(0, 100); softPwmWrite(1, 100);
            softPwmWrite(2, 100); softPwmWrite(3, 100);
            if (t > 0.8)
                estado = 2;
            break;
        // proximo estado de acordo com o digito lido (convencao A)
        case 2:
            if (card == 0)
                estado = 0;
            else{
                if (digito == 0 || digito == 1 || digito == 2 || digito == 3){
                    estado = 4;
                }
                else{
                    estado = 3;
                    digitoLido = digito;
                    t1 = timePoint();
                }
            }
            break;
        // curva acentuada para E ou D
        case 3:
            t2=timePoint();
            t=timeSpan(t1,t2);
            if (t > TIMERETURN || (card == 1 && escalaCard > 2)){
                estado = 5;
                t1=timePoint();
            } else{
                if (digitoLido ==  4 || digitoLido ==  5 || digitoLido ==  6)
                {   
                    // return E
                    softPwmWrite(0, 0); softPwmWrite(1, HALFSPEED);
                    softPwmWrite(2, HALFSPEED); softPwmWrite(3, 0);
                } else if (digitoLido ==  7 || digitoLido ==  8 || digitoLido ==  9){
                    // return D
                    softPwmWrite(0, HALFSPEED); softPwmWrite(1, 0);
                    softPwmWrite(2, 0); softPwmWrite(3, HALFSPEED);
                }
            }
            break;
        // parada e encerramento do programa
        case 4:
            softPwmWrite(0, 100); softPwmWrite(1, 100);
            softPwmWrite(2, 100); softPwmWrite(3, 100);
            return 0;
            break;
        // parada
        case 5:
            softPwmWrite(0, 100); softPwmWrite(1, 100);
            softPwmWrite(2, 100); softPwmWrite(3, 100);
            t2=timePoint();
            t=timeSpan(t1,t2);
            if (t > 0.8)
                estado = 0;
            break;

        default:
            break;
        }
    }
}

// controle dos motores para seguir cartao
void motores(int x, bool card, int escalacard){
    if (card == 0 || escalacard < 2){
        // parado
        softPwmWrite(0, 100); softPwmWrite(1, 100);
        softPwmWrite(2, 100); softPwmWrite(3, 100);
    } else if (card == 1){
        softPwmWrite(0, SPEEDL+(x/KX)); softPwmWrite(1, 0);
        softPwmWrite(2, SPEEDR-(x/KX)); softPwmWrite(3, 0);
    }
    
}