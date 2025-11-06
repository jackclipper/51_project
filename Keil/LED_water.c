#include <reg51.h>

void timedelay(int display_ms){
		unsigned int time = 120;
		int i,j;
		for(i=0; i<display_ms ;i++){
			for(j=0; j<time; j++){}
		}
}

void led(){
	int i;
	for(i=0; i<3; i++){
		P2=~(0x01<<i);
		timedelay(100);
	}
}

void main(){
	while(1){
		led();
	}
}