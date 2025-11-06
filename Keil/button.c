#include <reg51.h>

sbit button = P1^0;
sbit LED = P2^0;

void delay(int ms){
	int i,j;
	for(i=0; i<ms; i++){
		for(j=0; j<1200; j++){}
	}
}

void main(){
	while(1){
			if(button == 0){
				int k;
				for(k=0; k<5; k++){
					LED = 0;
					delay(100);
					LED = 1;
					delay(100);
				}
			}
	}

}