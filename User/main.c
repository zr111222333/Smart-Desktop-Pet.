#include "stm32f10x.h"                  // Device header 
#include "Delay.h"
#include "SERVO.H"
#include "OLED.H"
#include "SERIAL.H"
#include "ACTION.H"
#include "PWM.H"
#include "TIME4.h"
#include <math.h>
#include "BreathLED.h"
#include "PETACTION.h"
#include "AD.h"
#include "BLUETOOTH.h"
#include "OLED_Data.h"

uint8_t A,B;
int main(void)
{
	Time4_Init();
	OLED_Init();
	AD_Init();
	Action_Init();
	PetAction_Init();
	// Serial_SendData(0X56);
	
	
	
	while(1)
	{
		// 语音模块，蓝牙模块串口收发测试
		// if(Serial_GetRxFlag()==1)主循环调用后立马清零，导致TIM4中断里的语音模块动作数据无法采集
		//因为主循环用了一次Serial_GetRxFlag清零Serial_RxFlag导致一直以为没有新数据，导致无法显示
		// 只有在有新数据时才更新显示
		// if(PetAction_NewDataFlag == 1)
		// {
		// 	A=Serial_ReceiveData();
		// 	OLED_ShowString(1, 1, "        ",OLED_8X16);
		// 	OLED_ShowHexNum(1,1,A,4,OLED_8X16);
		// 	B=BlueTooth_ReceiveData();
		// 	OLED_ShowString(2, 1, "        ",OLED_8X16);
		// 	OLED_ShowHexNum(2,1,B,4,OLED_8X16);	
		// 	PetAction_NewDataFlag = 0;  // 清除新数据标志
		// }
		
			
	

		// 电源显示
        // OLED_ShowString(1, 1, "Vol:");
        // OLED_ShowNum(1, 5, (int)AD_GetVoltage(), 1);// 显示整数部分
        // OLED_ShowChar(1, 6, '.');
        // OLED_ShowNum(1, 7, (int)(AD_GetVoltage() * 10) % 10, 1); // 显示一位小数
        // OLED_ShowChar(1, 8, 'V');

		// OLED_ShowString(2, 1, "ADC:");
		// OLED_ShowNum(2, 5, (int)AD_GetFilteredADC(), 4);// 显示 AD 原始值（0-4095，最多4位）	
		// OLED_ShowBinNum(3, 1, (int)AD_GetFilteredADC(), 12);// 显示 AD 原始值（二进制，12位）	
		
		// OLED_ShowString(4, 1, "USART Rx:");
		// OLED_ShowHexNum(4, 10, Serial_ReceiveData(), 4);// 显示串口接收到的数据
		PetAction_SelectAction();
		

	}		
}
