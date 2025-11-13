#include <reg51.h>

sbit _7SEG1=P2^0;
sbit _7SEG2=P2^1;
sbit _7SEG3=P2^2;
sbit _7SEG4=P2^3;

static unsigned char segCode[10]=
{0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
0xd8, 0x80, 0x90}; //共阳型字模

unsigned char string[]={1,8,9,5};

void timedelay(int ms){
	unsigned int time = 120;
	int i,j;
	for(i=0; i<ms; i++){
		for(j=0; j<time; j++){}
	}
}

void main(){
	unsigned int k;
	while(1){
		for(k=0; k<4; k++){
			_7SEG1=0;
			_7SEG2=0;
			_7SEG3=0;
			_7SEG4=0; //先关显示！
			P0=segCode[string[k]];
			switch(k){
				case 0: _7SEG1=1; break;
				case 1: _7SEG2=1; break;
				case 2: _7SEG3=1; break;
				case 3: _7SEG4=1; break;
			}
			timedelay(10);
		}
		
	}
}