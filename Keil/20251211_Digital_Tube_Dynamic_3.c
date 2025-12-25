#include <reg51.h>
//在动态显示数码管上实现跑动点的代码
sbit Key3=P3^2;
sbit Key4=P3^3;

unsigned char x=7,y=0,x_temp,y_temp,size=3, type=0, type_temp=0;

void timedelay(int ms){
    unsigned int i,j;
    for(i=0; i<ms; i++){
        for(j=0; j<120; j++){
        }
    }
}

void place_change_clockwise_type0(){
    if(x == 0){
        if(y == 0){
            y++;
        }
        else if(y == 1){
            y=6;
        }
        else{
            x++;
        }
    }
    else if(x == 7){
        if(y == 6){
            y=4;
        }
        else if(y == 0){
            x--;
        }
        else{
            y=3;
            type=1;
        }
    }
    else if(y == 6){
        x++;
    }
    else if(y == 0){
        x--;
    }
}

void place_change_clockwise_type1(){
    if(x == 0){
        if(y == 3){
            y--;
        }
        else if(y == 2){
            y=6;
        }
        else{
            x++;
        }
    }
    else if(x == 7){
        if(y == 6){
            y=5;
        }
        else if(y == 3){
            x--;
        }
        else{
            y=0;
            type=0;
        }
    }
    else if(y == 6){
        x++;
    }
    else if(y == 3){
        x--;
    }
}

//only operate x_temp
void place_change_anticlockwise_type0(){
    if(x_temp == 0){
        if(y_temp == 0){
            x_temp++;
        }
        else if(y_temp == 1){
            y_temp--;
        }
        else{
            y_temp=1;
        }
    }
    else if(x_temp == 7){
        if(y_temp == 0){
            y_temp=5;
            type_temp=1;
        }
        else if(y_temp == 4){
            y_temp=6;
        }
        else{
            x_temp--;
        }
    }
    else if(y_temp == 6){
        x_temp--;
    }
    else if(y_temp == 0){
        x_temp++;
    }
}

//only operate x_temp
void place_change_anticlockwise_type1(){
    if(x_temp == 0){
        if(y_temp == 3){
            x_temp++;
        }
        else if(y_temp == 2){
            y_temp++;
        }
        else{
            y_temp=2;
        }
    }
    else if(x_temp == 7){
        if(y_temp == 5){
            y_temp++;
        }
        else if(y_temp == 3){
            y_temp=4;
            type_temp=0;
        }
        else{
            x_temp--;
        }
    }
    else if(y_temp == 6){
        x_temp--;
    }
    else if(y_temp == 3){
        x_temp++;
    }
}

void keycheck(){
    if(Key3 == 0){//reduce
        timedelay(350);
        if(size>1){
            size--;
        }
    }
    if(Key4 == 0){//increase
        timedelay(350);
        if(size<18){
            size++;
        }
    }
}

//x = 0 to 7, y = 0 to 5
void show(unsigned char x, unsigned char y){
    P2 &= 0xe3; //不更改P2端口其它输出情况的同时，使三口改变输出
    P2 |= (x<<2);
    P0=0x01 << y;
    timedelay(10);
}

void show_tail(){
    unsigned char i;
    x_temp=x;
    y_temp=y;
    type_temp=type;
    for(i=size;i>1;i--){
        if(type_temp == 0){
            place_change_anticlockwise_type0();
        }
        else{
            place_change_anticlockwise_type1();
        }
        show(x_temp,y_temp);
    }
}

void main(){
    while(1){
        unsigned char i;
        for(i=10000; i>0; i--){
            keycheck();
            show(x,y);
            show_tail();
        }
        if(type == 0){
            place_change_clockwise_type0();
        }
        else{
            place_change_clockwise_type1();
        }
    }
}