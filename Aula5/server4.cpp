// servidor1.cpp

#include "projeto.hpp"
#include <cekeikon.h>
#include <wiringPi.h>
#include <softPwm.h>
#define HALFSPEED 75
#define SPEED  90
#define SPEEDL 50
#define SPEEDR 90

Mat_<COR> rotate(Mat src, double angle)   //rotate function returning mat object with parametres imagefile and angle    
{
    Mat dst;      //Mat object for output image file
    Point2f pt(src.cols/2., src.rows/2.);          //point from where to rotate    
    Mat r = getRotationMatrix2D(pt, angle, 1.0);      //Mat object for storing after rotation
    warpAffine(src, dst, r, Size(src.cols, src.rows));  ///applie an affine transforation to image.
    return dst;         //returning Mat object for output image file
}

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
    Mat_<COR> img_rotate;
    Mat_<COR> img_send;
    uint32_t a;

    VideoCapture cam(0);
    if (!cam.isOpened()) erro("Erro: Abertura de webcam 0.");
    cam.set(CV_CAP_PROP_FRAME_WIDTH,320);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT,240);

    server.receiveUint(a);
    while (a == 100){
        uint32_t card = 0;
        uint32_t escalaCard = 0;
        uint32_t x = 0;
        cam >> img;
        img.convertTo(img_rotate, -1, 1, 50); //increase the brightness by 50
        img_send = rotate(img_rotate, 180);
        server.sendImgComp(img_send);
        // recebe dados do cartao
        server.receiveUint(card);
        server.receiveUint(x);
        server.receiveUint(escalaCard);
        // controla os motores para seguir o cartao
        motores(x, card, escalaCard);
        server.receiveUint(a);
    }
}

void motores(int x, bool card, int escalacard){
    if (card == 0 || escalacard < 2){
        // parado
        softPwmWrite(0, 100); softPwmWrite(1, 100);
        softPwmWrite(2, 100); softPwmWrite(3, 100);
    } else if (card == 1){
        // altera a velocidade dos motores de acordo com a posicao do cartao
        softPwmWrite(0, SPEEDL+(x/1.5)); softPwmWrite(1, 0);
        softPwmWrite(2, SPEEDR-(x/1.5)); softPwmWrite(3, 0);
    }
    
}