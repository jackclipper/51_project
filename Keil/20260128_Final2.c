#include <reg51.h>

// 引脚定义
sbit KEY_R  = P3^0; // 右
sbit KEY_L  = P3^1; // 左
sbit KEY_U  = P3^2; // 上 (INT0)
sbit KEY_D  = P3^3; // 下

sbit DS     = P3^4; // 74HC595 数据输入
sbit ST_CP  = P3^5; // 74HC595 锁存时钟
sbit SH_CP  = P3^6; // 74HC595 移位时钟

// 全局变量
#define MAX_LEN 32

// 蛇身坐标信息
unsigned char snakeX[MAX_LEN];
unsigned char snakeY[MAX_LEN];
unsigned char snakeLen;

// 方向: 0:右, 1:左, 2:上, 3:下
unsigned char dir;     
unsigned char nextDir; 

// 显存缓冲
// 存储该行的列数据
// 显存索引0对应顶行 (Y=0)
// 数据位7对应最左列 (X=0)
unsigned char displayBuffer[8]; 

unsigned char timerCount = 0;
bit gameOver = 0;

// 函数声明
void DelayMs(unsigned int ms)
{
    unsigned int x, y;
    for(x = ms; x > 0; x--)
        for(y = 110; y > 0; y--);
}
void HC595_Send(unsigned char dat);
void UpdateDisplayBuffer();
void DisplayScan();
void GameInit();
void MoveSnake();
void CheckButtons();
void GameOverAnimation();

// 主函数
void main()
{
    // 定时器0初始化
    TMOD = 0x01; 
    TH0 = (65536 - 50000) / 256; 
    TL0 = (65536 - 50000) % 256;
    EA = 1;  
    ET0 = 1; 
    TR0 = 1; 

    // 外部中断0配置
    IT0 = 1; // 下降沿触发
    EX0 = 0; // 游戏初始化时关闭

    GameInit();

    while(1)
    {
        if (!gameOver)
        {
            CheckButtons();    
            DisplayScan();     
        }
        else
        {
            DisplayScan();
        }
    }
}

// 中断服务
void Timer0_ISR() interrupt 1
{
    TH0 = (65536 - 50000) / 256; // 50ms
    TL0 = (65536 - 50000) % 256;

    if (!gameOver)
    {
        timerCount++;
        if (timerCount >= 20) // 1s 移动一次
        {
            timerCount = 0;
            MoveSnake();
        }
    }
}

void External0_ISR() interrupt 0
{
    // 重置游戏数据
    GameInit();

    while (KEY_U == 0)
    {
        DisplayScan(); 
    }
    EX0 = 0; // 关闭中断
    TR0 = 1; 
    IE0 = 0; // 清除中断标志
}

// ===== 游戏逻辑层 =====

void GameInit()
{
    unsigned char i;
    
    // 长度5，位置左上角，向右移动
    snakeLen = 5;
    dir = 0; 
    nextDir = 0;
    
    for(i=0; i<snakeLen; i++)
    {
        snakeX[i] = 4 - i; 
        snakeY[i] = 0;     
    }

    gameOver = 0;
    timerCount = 0;
    UpdateDisplayBuffer();
}

// 按键防反向
void CheckButtons()
{
    if(KEY_R == 0) { if(dir != 1) nextDir = 0; }
    if(KEY_L == 0) { if(dir != 0) nextDir = 1; }
    if(KEY_U == 0) { if(dir != 3) nextDir = 2; }
    if(KEY_D == 0) { if(dir != 2) nextDir = 3; }
}

void MoveSnake()
{
    signed char newHeadX, newHeadY;
    unsigned char i;

    dir = nextDir;
    newHeadX = snakeX[0];
    newHeadY = snakeY[0];

    switch(dir)
    {
        case 0: newHeadX++; break; // 右
        case 1: newHeadX--; break; // 左
        case 2: newHeadY--; break; // 上
        case 3: newHeadY++; break; // 下
    }

    // 撞墙检测
    if (newHeadX < 0 || newHeadX > 7 || newHeadY < 0 || newHeadY > 7)
    {
        GameOverAnimation(); return;
    }

    // 撞自身检测
    for (i = 0; i < snakeLen; i++)
    {
        if (snakeX[i] == newHeadX && snakeY[i] == newHeadY)
        {
            GameOverAnimation(); return;
        }
    }

    // 移动身体
    for (i = snakeLen; i > 0; i--)
    {
        snakeX[i] = snakeX[i-1];
        snakeY[i] = snakeY[i-1];
    }

    // 更新头部
    snakeX[0] = newHeadX;
    snakeY[0] = newHeadY;

    UpdateDisplayBuffer();
}

// 映射函数：将逻辑坐标转换为物理显存
void UpdateDisplayBuffer()
{
    unsigned char x, y, i;
    
    // 清空显存
    for(i=0; i<8; i++) displayBuffer[i] = 0x00;

    for(i=0; i<snakeLen; i++)
    {
        x = snakeX[i];
        y = snakeY[i];
        
        if(x < 8 && y < 8)
        {
            displayBuffer[y] |= (1 << (7 - x));
        }
    }
}

void GameOverAnimation()
{
    unsigned char y, i;
    gameOver = 1; 
    
    // 逐行点亮动画
    for (y = 8; y > 0; y--) 
    {
        displayBuffer[y-1] = 0xFF; 
        
        for (i = 0; i < 30; i++) 
        {
            DisplayScan();
            DelayMs(2);
        }
    }

    IE0 = 0; // 清除中断标志
    EX0 = 1; // 开启中断
}

// ===== 驱动层 =====

void HC595_Send(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80) DS = 1; else DS = 0;
        SH_CP = 0; SH_CP = 1; 
        dat <<= 1;
    }
    ST_CP = 0; ST_CP = 1; 
}

void DisplayScan()
{
    unsigned char row;
    unsigned char rowSelect;

    // 扫描每一行 (Y轴)
    for (row = 0; row < 8; row++)
    {
        // 行选
        rowSelect = (1 << (7 - row));
        HC595_Send(rowSelect);

        // 列选
        P0 = ~displayBuffer[row]; 

        DelayMs(1); 
        P0 = 0xFF;
    }
}