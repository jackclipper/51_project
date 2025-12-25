#include <reg51.h>
#define KEY_PORT P3
unsigned char keyVal;
unsigned char num[]={0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90, 
0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0xff};

void timedelay(int ms){
    unsigned int i,j;
    for(i=0; i<ms; i++){
        for(j=0; j<120; j++){
        }
    }
}

unsigned char keyscan(){
    unsigned char row,col;
    KEY_PORT = 0x0f; //行线全为1
    if(KEY_PORT!=0x0f){ //检测
        timedelay(20);
        if(KEY_PORT!=0x0f){
            row=KEY_PORT;
            KEY_PORT=0xf0; //列线全为1
            col=KEY_PORT;
            while(KEY_PORT!=0xf0);
            return(row|col);
        }
    }
}

void read_key(){
    unsigned char key;
    key = keyscan();
    switch (key){
        case 0xee: keyVal=0; break;
        case 0xde: keyVal=1; break;
        case 0xbe: keyVal=2; break;
        case 0x7e: keyVal=3; break;
        case 0xed: keyVal=4; break;
        case 0xdd: keyVal=5; break;
        case 0xbd: keyVal=6; break;
        case 0x7d: keyVal=7; break;
        case 0xeb: keyVal=8; break;
        case 0xdb: keyVal=9; break;
        case 0xbb: keyVal=10; break;
        case 0x7b: keyVal=11; break;
        case 0xe7: keyVal=12; break;
        case 0xd7: keyVal=13; break;
        case 0xb7: keyVal=14; break;
        case 0x77: keyVal=15; break;
    }
}

void main(){
    keyVal = 16;
    while(1){
        read_key();
        P1=num[keyVal];
    }
}