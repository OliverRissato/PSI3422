//blink3.cpp
//compila blink3 -w
#include <wiringPi.h>
int main () {
	wiringPiSetup () ;
	pinMode (0, OUTPUT) ;
	pinMode (1, OUTPUT) ;
	pinMode (2, OUTPUT) ;
	pinMode (3, OUTPUT) ;
	for (int i=0; i<99; i++) {
		digitalWrite (0, HIGH) ;
		digitalWrite (1, LOW) ;
		digitalWrite (2, HIGH) ;
		digitalWrite (3, LOW) ;
		delay (2000) ;
		digitalWrite (0, LOW) ;
		digitalWrite (1, HIGH) ;
		digitalWrite (2, LOW) ;
		digitalWrite (3, HIGH) ;
		delay (2000) ;
	}
	digitalWrite (0, LOW) ;
	digitalWrite (1, LOW) ;
	digitalWrite (2, LOW) ;
	digitalWrite (3, LOW) ;
}
