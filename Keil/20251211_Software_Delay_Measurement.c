#include <reg51.h>
//使用计数器中断实现对软件延时函数的测量

sbit KEY3=P3^2;

unsigned char segCode[10]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f};
unsigned char disp[8];
unsigned int time=0;
bit time_ready = 0;

//将当前时间拆分，用于数码管显示
void calc(){
    unsigned int i, temp=time;
    for(i=0; i<8; i++){
        disp[i]=segCode[temp%10];
        temp/=10;
        if(temp==0){
            break;
        }
    }
}

//被测量的延时函数
void delay(unsigned int a){
    while(a--);
}

void show(){
    unsigned char i;
    for(i=0; i<8; i++){
        P2 &= 0xe3; //不更改P2端口其它输出情况的同时，使三口改变输出
        P2 |= (i<<2);
        P0=disp[i];
        delay(120);
    }
}

void init(){
    EA=1;
    EX0=1;
    IT0=1;

    ET0=0;
    TMOD=0x01; //定时器模式、16位计数器
}

//外部中断服务程序，用于软件激活定时器中断
void key_check() interrupt 0{
    if (time_ready == 1) return;
    TL0=0x00; //设定初值
    TH0=0x00;
    TR0=1;
    delay(1000);
    TR0=0;
    time = time + TL0 + (TH0 << 8);
    time_ready = 1;
}

void main(){
    init();
    while(1){
        if(time_ready){
            calc();
        }
        show();
    }
}