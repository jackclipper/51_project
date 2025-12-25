#include <reg51.h>

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    
    // 粗略估算：当晶振为12MHz时，
    // for(j = 0; j < 1200; j++) 的执行时间约等于 1 毫秒
    // 这个数值 (1200) 需要根据实际测试和晶振频率来精确调整
    
    for (i = 0; i < ms; i++) // 外层循环控制毫秒数
    {
        for (j = 0; j < 100; j++); // 内层循环消耗时间
    }
}

void main(){
	while(1){
		P2=0xFF;
		delay_ms(1000);
		P2=0x00;
		delay_ms(1000);
	}
}