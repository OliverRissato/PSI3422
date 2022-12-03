//paramotor.cpp
//compila paramotor -w
#include <wiringPi.h>
int main () {
 wiringPiSetup () ;
 pinMode (0, OUTPUT) ;
 pinMode (1, OUTPUT) ;
 pinMode (2, OUTPUT) ;
 pinMode (3, OUTPUT) ;
 digitalWrite (0, LOW) ;
 digitalWrite (1, LOW) ;
 digitalWrite (2, LOW) ;
 digitalWrite (3, LOW) ;
}