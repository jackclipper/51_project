#include <REG51.H>

sbit KEY1 = P3^0; //设置按键
sbit KEY_INC = P3^2; // 增加键
sbit KEY_DEC = P3^3; // 减少键

unsigned char segCode[10]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x6f};
unsigned char disp[8]={0x3f, 0x06, 0, 0, 0, 0, 0, 0};
unsigned int countdown = 10, countdown_init = 10;
bit blink_flag=1, setting_mode=0, start_flag=0, pause_flag=0;
unsigned char tick50ms=0;

//软件延时
void timedelay(int ms){
    unsigned int i,j;
    for(i=0; i<ms; i++){
        for(j=0; j<120; j++){
        }
    }
}

void update_display_from_count(unsigned int val){
    unsigned char i;
    for(i=0; i<8; i++){
        disp[i] = segCode[val % 10];
        val /= 10;
        if(val == 0){
            disp[i+1] = 0; //高位补0
            break;
        }
    }
}

void show(){
    unsigned char i;
    for(i=0; i<8; i++){
        P2 &= 0xe3; //不更改P2端口其它输出情况的同时，使三口改变输出
        P2 |= (i<<2);
        if(setting_mode == 1 && blink_flag == 0){
            P0=0x00;
        }
        else if(setting_mode == 0 && start_flag == 1 && pause_flag == 1){
            if(i == 3){
                P0=0x36; //显示暂停符号
            }
            else{
                P0=0x00;
            }
        }
        else if(setting_mode == 0 && start_flag == 0 && pause_flag == 1){
            if(i == 7){
                P0=0x49; //显示停止符号
            }
            else{
                P0=0x00;
            }
        }
        else{
            P0=disp[i];
        }
        timedelay(1);
    }
}

void check(){
    //KEY1：进入设置
    if(KEY1 == 0){
        timedelay(20);
        if(KEY1 == 0){
            while(KEY1 == 0); //等待按键释放
            timedelay(20);
            setting_mode = !setting_mode;
            if(setting_mode == 0){
                blink_flag = 1; //退出设置模式时，确保显示不闪烁
            }
            else{
                blink_flag = 0; //进入设置模式时，开始闪烁
            }
        }
    }
    if(countdown == 0){                  
        pause_flag = 1;
        start_flag = 0;
    }
}

void timer0() interrupt 1{
    TL0=(65535-50000)%256;
    TH0=(65535-50000)/256;
    TF0=0; //清除定时器0溢出标志
    TR0=1; //启动定时器0
    tick50ms++;
    if(tick50ms >= 10 && setting_mode == 1){
        tick50ms=0;
        blink_flag = !blink_flag; //切换闪烁状态
    }
    else if(tick50ms >= 20 && setting_mode == 0 && start_flag == 1){
        tick50ms=0;
        if(pause_flag == 1){
            return;
        }
        if(countdown > 0){
            countdown--;
            update_display_from_count(countdown);
        }
    }
    else if(tick50ms >= 40 && setting_mode == 0 && start_flag == 0 && pause_flag == 1){
        tick50ms=0;
        countdown = countdown_init;
        start_flag=0;
        pause_flag=0;
        update_display_from_count(countdown);
    }
    else if(tick50ms >= 50){
        tick50ms=0;
    }
}

void external0() interrupt 0{
    if(setting_mode == 1){
        if(countdown < 99999999) countdown++;
        update_display_from_count(countdown);
        countdown_init = countdown;
    }
    else if(pause_flag == 1){
        return;
    }
    else{
        start_flag = 1;
    }
}

void external1() interrupt 2{
    if(setting_mode == 1){
        if(countdown > 0) countdown--;
        update_display_from_count(countdown);
        countdown_init = countdown;
    }
    else if(start_flag == 0){
        return;
    }
    else{
        pause_flag = !pause_flag;
    }
}

void init(){
    EA=1; //开总中断
    EX0=1; //开外中断0
    ET0=1; //开定时器0中断
    EX1=1; //开外中断1

    IT0=1; //设置外中断0为边沿触发方式
    IT1=1; //设置外中断1为边沿触发方式
    TMOD=0x01; //设置定时器0为模式1
    TL0=(65535-50000)%256;
    TH0=(65535-50000)/256;
    TR0=1; //启动定时器0
    update_display_from_count(countdown);
}

void main(){
    init();
    while(1){
        show();
        check();
    }
}