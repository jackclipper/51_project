#include <reg51.h>
#include <string.h>

#define uchar unsigned char
#define uint  unsigned int

// 引脚定义
sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;
sbit LED_G = P3^0; // 绿灯：正确
sbit LED_R = P3^1; // 红灯：错误
sbit BEEP  = P3^2; // 扬声器驱动

// 全局变量
uchar current_pwd[5] = "1234";   // 当前密码（初始为1234）
uchar input_pwd[5];              // 输入缓冲
uchar error_cnt = 0;             // 累计错误次数

// 延时函数
void delay(uint ms) {
    uint i, j;
    for(i=ms; i>0; i--)
        for(j=110; j>0; j--);
}

// 扬声器
void alarm_sound(uint duration_ms) {
    uint i;
    for(i=0; i<duration_ms; i++) {
        BEEP = !BEEP; 
        delay(1); 
    }
}

// LCD1602驱动
void write_cmd(uchar cmd) { RS=0; RW=0; P0=cmd; EN=1; delay(1); EN=0; }
void write_dat(uchar dat) { RS=1; RW=0; P0=dat; EN=1; delay(1); EN=0; }
void lcd_init() {
    write_cmd(0x38); write_cmd(0x0c); write_cmd(0x06); write_cmd(0x01);
}
void display_str(uchar row, uchar col, uchar *s) {
    if(row == 0) write_cmd(0x80 + col);
    else write_cmd(0xC0 + col);
    while(*s) write_dat(*s++);
}

// 矩阵键盘扫描
uchar scan_key() {
    uchar row, col, tmp;
    P1 = 0xF0; // 高四位列输入，低四位行输出
    if(P1 != 0xF0) {
        delay(10); // 消抖
        if(P1 != 0xF0) {
            // 行检测
            tmp = P1;
            if(tmp==0xE0) row=0; else if(tmp==0xD0) row=1;
            else if(tmp==0xB0) row=2; else if(tmp==0x70) row=3;
            // 列检测
            P1 = 0x0F;
            tmp = P1;
            if(tmp==0x0E) col=0; else if(tmp==0x0D) col=1;
            else if(tmp==0x0B) col=2; else if(tmp==0x07) col=3;
            
            while(P1 != 0x0F); // 等待按键释放
        
            if(row==0 && col==0) return '7'; if(row==0 && col==1) return '8'; if(row==0 && col==2) return '9';
            if(row==1 && col==0) return '4'; if(row==1 && col==1) return '5'; if(row==1 && col==2) return '6'; if(row==1 && col==3) return '*';
            if(row==2 && col==0) return '1'; if(row==2 && col==1) return '2'; if(row==2 && col==2) return '3';
            if(row==3 && col==0) return 'C'; if(row==3 && col==1) return '0'; if(row==3 && col==2) return 'V';
        }
    }
    return 0xFF;
}

// 修改密码逻辑
void change_pwd_mode() {
    uchar i = 0, k;
    uchar n1[5], n2[5];
    
    write_cmd(0x01);
    display_str(0, 0, "New Pwd:");
    while(i < 4) {
        k = scan_key();
        if(k >= '0' && k <= '9') {
            n1[i++] = k;
            write_dat('*');
        }
    }
    n1[4] = '\0';
    while(scan_key() != 'V'); // 按下确定

    write_cmd(0x01);
    display_str(0, 0, "Input Again:");
    i = 0;
    while(i < 4) {
        k = scan_key();
        if(k >= '0' && k <= '9') {
            n2[i++] = k;
            write_dat('*');
        }
    }
    n2[4] = '\0';

    if(strcmp(n1, n2) == 0) {
        strcpy(current_pwd, n1);
        display_str(1, 0, "Pwd is changed!");
    } else {
        display_str(1, 0, "Error!");
    }
    delay(2000);
}

// 主函数
void main() {
    uchar i = 0, k;
    lcd_init();
    LED_G = 1; LED_R = 1; BEEP = 0;

    while(1) {
        write_cmd(0x01);
        display_str(0, 0, "Input: ");
        i = 0;
        memset(input_pwd, 0, 5);

        while(i < 4) {
            k = scan_key();
            if(k >= '0' && k <= '9') {
                input_pwd[i++] = k;
                display_str(0, 7+i, "*");
            }
            else if(k == 'C' && i > 0) { // 回退键
                write_cmd(0x80 + 7 + i); write_dat(' '); i--; write_cmd(0x80 + 7 + i);
            }
            else if(k == '*') { // 修改键
                change_pwd_mode();
                i = 9; // 强制重新开始主循环
            }
        }

        if(i == 4) { // 输入满4位
            while(scan_key() != 'V'); // 等待确认键
            if(strcmp(input_pwd, current_pwd) == 0) {
                display_str(1, 0, "Correct!");
                LED_G = 0; delay(1000); LED_G = 1;
                error_cnt = 0;
            } else {
                display_str(1, 0, "Error!");
                LED_R = 0; alarm_sound(1000); LED_R = 1;
                error_cnt++;
                if(error_cnt >= 3) {
                    display_str(1, 0, "Locked 3s!");
                    alarm_sound(3000);
                    error_cnt = 0;
                }
            }
        }
    }
}