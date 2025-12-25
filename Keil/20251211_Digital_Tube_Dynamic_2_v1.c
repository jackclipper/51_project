#include <reg51.h>
//基本的动态显示数码管，按键实现数字增加
sbit Key3=P3^2;
sbit Key4=P3^3;

unsigned char segCode[10]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f};
unsigned char disp[8];

unsigned char hour=23, minute=59, second=50;

void calc(){
    disp[0]=segCode[second%10];
    disp[1]=segCode[second/10];
    disp[2]=0x40;
    disp[3]=segCode[minute%10];
    disp[4]=segCode[minute/10];
    disp[5]=0x40;
    disp[6]=segCode[hour%10];
    disp[7]=segCode[hour/10];
}

void timedelay(int ms){
    unsigned int i,j;
    for(i=0; i<ms; i++){
        for(j=0; j<120; j++){
        }
    }
}

void timefly()
{
    if (hour == 23)
    {
        if (minute == 59)
        {
            if (second == 59)
            {
                second = 0;
                minute = 0;
                hour = 0;
            }
            else
            {
                second++;
            }
        }
        else
        {
            if (second == 59)
            {
                second = 0;
                minute++;
            }
            else
            {
                second++;
            }
        }
    }
    else
    {
        if (minute == 59)
        {
            if (second == 59)
            {
                second = 0;
                minute = 0;
                hour++;
            }
            else
            {
                second++;
            }
        }
        else
        {
            if (second == 59)
            {
                second = 0;
                minute++;
            }
            else
            {
                second++;
            }
        }
    }
}

void show(){
    unsigned char i;
    for(i=0; i<8; i++){
        P2 &= 0xe3; //不更改P2端口其它输出情况的同时，使三口改变输出
        P2 |= (i<<2);
        P0=disp[i];
        timedelay(1);
    }
}

void keycheck(){
    if(Key3 == 0){//写错了，没有减时间的函数
        timedelay(100);
        timefly();
    }
    if(Key4 == 0){
        timedelay(100);
        timefly();
    }
}

void main(){
    unsigned char i;
    while(1){
        for(i=0; i<150; i++){
            keycheck();
            calc();
            show();
        }
        timefly();
    }
}