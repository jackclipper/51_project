#include <reg51.h>

sbit button0 = P1^0;
sbit button1 = P1^1;
sbit LED = P2^0;

void delay(int ms){
	int i,j;
	for(i=0; i<ms; i++){
		for(j=0; j<1200; j++){}
	}
}

void blink(unsigned int p){
	int k;
	for(k=0; k<p; k++){
		LED = 0;
		delay(100);
		LED = 1;
		delay(100);
	}
}

void main(){
	while(1){
			if(button0 == 0){
				blink(1);
			}
			else if(button1 == 0){
				blink(2);
			}
	}

}