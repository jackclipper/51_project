#include <reg51.h>
#define dutytime_us 200
#define freetime_us 800
sbit Output=P2^0;
unsigned char dutyflag;

void init_timer(){
    TMOD=0X01;
    TH0=(65536-dutytime_us)/256;
    TL0=(65536-dutytime_us)%256;
    EA=1;
    ET0=1;
    TR0=1;
    dutyflag=1;
    Output=1;
}

void main(){
    init_timer();
    while(1);
}

void timer() interrupt 1{
    if(dutyflag){
        TH0=(65536-freetime_us)/256;
        TL0=(65536-freetime_us)%256;
        dutyflag=0;
        Output=0;
    }
    else{
        TH0=(65536-dutytime_us)/256;
        TL0=(65536-dutytime_us)%256;
        dutyflag=1;
        Output=1;
    }
}