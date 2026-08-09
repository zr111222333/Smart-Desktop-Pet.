#include "stm32f10x.h"                  // Device header
#include "SERVO.H"
#include "SERIAL.H"
#include "Delay.h"
#include "OLED.h"
#include "TIME4.h"
#include "BREATHLED.h"
#include "PWM.h"
#include "PETACTION.h"
#include "AD.h"
#include "BLUETOOTH.h"

void Action_Init(void)
{
	Serial_Init();
	Servo_Init();
}



// 可打断的等待函数
// 在等待舵机运动完成的过程中，如果收到新命令就立即返回，实现动作可打断
static void Action_WaitUntilMovingComplete(void)
{
	while(Servo_AllIsMoving() == 1)
	{
		// 检查是否有新命令，有的话立即返回，不再继续等待
		if(PetAction_ActionState != 0)
		{
			return;
		}
	}
}

//起立
void Action_Stand(void)
{
	Servo_SetAngleData(90,90,90,90,180);
}

//趴下
void Action_LieDown(void)
{
	Servo_SetAngleData(180,180,180,180,0);

}

//蹲下
void Action_SitDown(void)
{
	Servo_SetAngleData(90,90,135,135,180);

}

//前进
void Action_Run(void)
{
	for(uint8_t i=0;i<4;i++)
	{
			//简单的“迈左边 -> 迈右边”两步循环
			Servo_SetAngleData(135, 45, 45, 135, 180); 
			Action_WaitUntilMovingComplete();	
			Servo_SetAngleData(45, 135, 135, 45, 180);
			Action_WaitUntilMovingComplete();
	}	
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
		
}

//退后
void Action_Back(void)
{
	for(uint8_t i=0;i<4;i++)
	{	
			Servo_SetAngleData(45, 135, 135, 45, 180);
			Action_WaitUntilMovingComplete();

			Servo_SetAngleData(135, 45, 45, 135, 180);
			Action_WaitUntilMovingComplete();
			
	}	
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();

}	

//左转
void Action_TurnLeft(void)
{
	for(uint8_t i=0;i<4;i++)
	{
			Servo_SetAngleData(45,135,135,45,180);
			Action_WaitUntilMovingComplete();
			Servo_SetAngleData(135,45,45,135,180);
			Action_WaitUntilMovingComplete();
	
	}	
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();

}

//右转
void Action_TurnRight(void)
{
	for(uint8_t i=0;i<4;i++)
	{
			Servo_SetAngleData(135,45,45,135,180);
			Action_WaitUntilMovingComplete();

			Servo_SetAngleData(45,135,135,45,180);
			Action_WaitUntilMovingComplete();
		
	}	
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
}

//摇摆
void Action_Shake(void)
{
    for(uint8_t i=0;i<4;i++)
    {
            // --- 向左摇摆
            Servo_SetAngleData(35, 90, 145, 90, 180);
			Action_WaitUntilMovingComplete();

            // --- 向右摇摆
            Servo_SetAngleData(90, 35, 90, 145, 180);
			Action_WaitUntilMovingComplete();

	}	
            Servo_SetAngleData(90, 90, 90, 90, 180);
            Action_WaitUntilMovingComplete();
	 
}

//走快点-----------------------------------
void Action_RunFast(void)
{
	K=0.5; // 修改全局Speed变量来加快速度
	for(uint8_t i=0;i<4;i++)
	{
			//简单的"迈左边 -> 迈右边"两步循环
			Servo_SetAngleData(135, 45, 45, 135, 180); 
			Action_WaitUntilMovingComplete();
			
			Servo_SetAngleData(45, 135, 135, 45, 180);
			Action_WaitUntilMovingComplete();
	
	}	
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
	 
	K=0.1;  // 恢复默认速度
}


//快点摇摆
void Action_ShakeFast(void)
{
	K=0.5; // 修改全局Speed变量来加快速度
    for(uint8_t i=0;i<4;i++)
    {
            // --- 向左摇摆
            Servo_SetAngleData(35, 90, 145, 90, 180);
			Action_WaitUntilMovingComplete();
			
            // --- 向右摇摆
            Servo_SetAngleData(90, 35, 90, 145, 180);
			Action_WaitUntilMovingComplete();
			
	}	
    Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
 
	K=0.1;  // 恢复默认速度
}

//摇尾巴--------------------------------------------------------
void Action_TailWag(void)
{
	K=0.3; 
	Servo_TailEnable=1; // 使能尾巴摆动
	for(uint8_t i=0;i<4;i++)
	{
		Servo_SetAngleData(90, 90, 90, 90, 0);
		Action_WaitUntilMovingComplete();			
		Servo_SetAngleData(90, 90, 90, 90, 180);
		Action_WaitUntilMovingComplete();
	}
	K=0.1; 
}

//停止摇尾巴
void Action_TailWagStop(void)
{
	Servo_SetAngle5(90); // 尾巴回中
	Action_WaitUntilMovingComplete();
	Servo_TailEnable=0; // 停止尾巴摆动
	
}

//向前跳
void Action_JumpForward(void)
{
	for(uint8_t i=0;i<4;i++)
	{
		// 蹲下准备跳（四条腿都缩短）
		Servo_SetAngleData(45, 45, 120, 120, 180);		
		Action_WaitUntilMovingComplete();

		// 向前跳起（四条腿伸展，产生跳跃动作）
		Servo_SetAngleData(120, 120, 60, 60, 180);
		Action_WaitUntilMovingComplete();
	
	}
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
}

//向后跳
void Action_JumpBackward(void)
{
	for(uint8_t i=0;i<4;i++)
	{
		Servo_SetAngleData(120, 120, 60, 60, 180);		
		Action_WaitUntilMovingComplete();
		Servo_SetAngleData(45, 45, 120, 120, 180);		
		Action_WaitUntilMovingComplete();
	
	}
	Servo_SetAngleData(90, 90, 90, 90, 180);
    Action_WaitUntilMovingComplete();
}

//打招呼
void Action_Hello(void)
{
	K=0.3; 
	for(uint8_t i=0;i<4;i++)
	{
		Servo_SetAngleData(90, 180, 90, 90, 180);
		Action_WaitUntilMovingComplete();
		
		Servo_SetAngleData(90, 90, 90, 90, 90);
		Action_WaitUntilMovingComplete();
				
	}
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
	
}

//伸懒腰
void Action_Stretch(void)
{
	//使用时间戳差值 
	uint16_t Start_TimeCount=0; // 定义开始时间
	Start_TimeCount=TimeCount; // 记录开始时间
	Servo_SetAngleData(90, 90, 150, 150, 180);
	Action_WaitUntilMovingComplete();

	Servo_SetAngleData(180, 180, 150, 150, 180);
	Action_WaitUntilMovingComplete();

	while(TimeCount-Start_TimeCount<=200); // 如果还没满2秒，等够2秒再收回
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
}

//伸腿
void Action_LegStretch(void)
{
	//使用时间戳差值 
	uint16_t Start_TimeCount=0; // 定义开始时间
	Start_TimeCount=TimeCount; // 记录开始时间
	Servo_SetAngleData(180, 180, 150, 150, 180);	
	Action_WaitUntilMovingComplete();

	Servo_SetAngleData(90, 90, 150, 150, 180);
	Action_WaitUntilMovingComplete();

	while(TimeCount-Start_TimeCount<=200); // 如果还没满2秒，等够2秒再收回
	Servo_SetAngleData(90, 90, 90, 90, 180);
	Action_WaitUntilMovingComplete();
}


//开灯
void Action_LightON(void)
{
	BreathLED_LEDState=1;
}

//关灯
void Action_LightOFF(void)
{
	BreathLED_LEDState=0;
}

//所有灯亮
void Action_AllLEDON(void)
{
	BreathLED_LEDState=0;

}

//所有灯灭
void Action_AllLEDOFF(void)
{
	BreathLED_LEDState=2;
}

//显示电压
void Action_ShowVoltage(void)
{
	// 电源显示
    OLED_ShowString(0, 0, "Vol:",OLED_8X16);
    OLED_ShowNum(32, 0, (int)AD_GetVoltage(), 1,OLED_8X16);// 显示整数部分
    OLED_ShowChar(40, 0, '.',OLED_8X16);
    OLED_ShowNum(48, 0, (int)(AD_GetVoltage() * 10) % 10, 1,OLED_8X16); // 显示一位小数
    OLED_ShowChar(56, 0, 'V',OLED_8X16);

	OLED_ShowString(0, 16, "ADC:",OLED_8X16);
	OLED_ShowNum(32, 16, (int)AD_GetFilteredADC(), 4,OLED_8X16);// 显示 AD 原始值（0-4095，最多4位）	
	OLED_ShowBinNum(0, 32, (int)AD_GetFilteredADC(), 12,OLED_8X16);// 显示 AD 原始值（二进制，12位）	
		
	OLED_ShowString(0, 48, "USART Rx:",OLED_8X16);
	OLED_ShowHexNum(64, 48, Serial_ReceiveData(), 4,OLED_8X16);// 显示串口接收到的数据
	
	 OLED_Update();// 把缓冲区数据通过I2C发送到OLED硬件
}
   


// 语音模块，蓝牙模块串口收发测试
void Action_SerialTest(void)
{
	// if(Serial_GetRxFlag()==1)主循环调用后立马清零，导致TIM4中断里的语音模块动作数据无法采集
	// 因为主循环用了一次Serial_GetRxFlag清零Serial_RxFlag导致一直以为没有新数据，导致无法显示
	// 只有在有新数据时才更新显示
	uint8_t A,B;
	if(PetAction_NewDataFlag == 1)
	{
		A=Serial_ReceiveData();
		OLED_ShowString(0, 1, "        ",OLED_8X16);
		OLED_ShowHexNum(0, 1, A, 4, OLED_8X16);
		B=BlueTooth_ReceiveData();
		OLED_ShowString(0, 3, "        ",OLED_8X16);
		OLED_ShowHexNum(0, 3, B, 4, OLED_8X16);	
		PetAction_NewDataFlag = 0;  // 清除新数据标志
		OLED_Update();// 把缓冲区数据通过I2C发送到OLED硬件
	}
}
		