#include <reg51.h>

void timedelay(int ms){
	unsigned int time = 120;
	int i,j;
	for(i=0; i<ms; i++){
		for(j=0; j<time; j++){}
	}
}

void tube(int num){
	if(num == 1){
		P2=0x06;
	}
	else if(num == 2){
		P2=0x5B;
	}
	else if(num == 3){
		P2=0x4F;
	}
	else if(num == 4){
		P2=0x66;
	}
	else if(num == 5){
		P2=0x6D;
	}
	else if(num == 6){
		P2=0x7D;
	}
	else if(num == 7){
		P2=0x07;
	}
	else if(num == 8){
		P2=0x7F;
	}
	else if(num == 9){
		P2=0xEF;
	}
	else if(num == 0){
		P2=0x3F;
	}
}

void main(){
	while(1){
		P3=1;
		tube(1);
		P3=0;
		timedelay(10000);
	}
}