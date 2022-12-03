//pwmroda4.cpp
//compila pwmroda4 -c -w
#include <cekeikon.h>
#include <wiringPi.h>
#include <softPwm.h>
int main() {
	wiringPiSetup();
	if (softPwmCreate(0, 0, 100)) erro("erro");
	if (softPwmCreate(1, 0, 100)) erro("erro");
	if (softPwmCreate(2, 0, 100)) erro("erro");
	if (softPwmCreate(3, 0, 100)) erro("erro");
	for (int i=0; i<2; i++) {
		softPwmWrite(0, 100); softPwmWrite(1, 0); delay(2000);
		softPwmWrite(0, 0); softPwmWrite(1, 100); delay(2000);
		softPwmWrite(0, 60); softPwmWrite(1, 0); delay(2000);
		softPwmWrite(0, 0); softPwmWrite(1, 60); delay(2000);
		softPwmWrite(0, 0); softPwmWrite(1, 0); delay(2000);
		softPwmWrite(2, 100); softPwmWrite(3, 0); delay(2000);
		softPwmWrite(2, 0); softPwmWrite(3, 100); delay(2000);
		softPwmWrite(2, 60); softPwmWrite(3, 0); delay(2000);
		softPwmWrite(2, 0); softPwmWrite(3, 60); delay(2000);
		softPwmWrite(2, 0); softPwmWrite(3, 0); delay(2000);
	}
}
