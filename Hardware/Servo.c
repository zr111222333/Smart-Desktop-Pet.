#include "stm32f10x.h"                 // Device header
#include "PWM.h"
#include <math.h>

//校准舵机初始角度的偏移量
#define LF1_OFFSET  0 
#define RF2_OFFSET  -10
#define LB3_OFFSET  10
#define RB4_OFFSET  -10
#define BH5_OFFSET  0 

volatile float Servo_CurrtntAngle[]={90.0,90.0,90.0,90.0,0.0};//当前角度
volatile int16_t Servo_TargetAngle[]={90,90,90,90,0};//目标角度
volatile float	K = 0.3;//加volatile全局变量，使可以在中断函数内可以修改
volatile uint8_t Servo_TailEnable = 1; // 尾巴摆动使能标志位

// ==========================================
void Servo_Init(void)
{
	PWM_Init();
}


// ==========================================
//更改CH1舵机角度
void Servo_SetAngle1_PWMData(int16_t Angle)//int16_t因为初始角度偏移量有符号，无符号的话在反方向会识别为正向10°
{//两个整数相除，结果也会是整数，小数部分会被直接丢弃，45 / 180=0.25直接为0

	Angle+=LF1_OFFSET;
	if(Angle > 180) Angle = 180;//防止越界 (0~180)，保护舵机过载堵转
	if(Angle < 0) Angle = 0;
	Angle=180-Angle;//解决舵机镜像偏移

	PWM_SetCompare1((float)(Angle)/180*(2500-500)+500);
	//0~180°对应CCR值500~2500区间设置
}

//更改CH2舵机角度
void Servo_SetAngle2_PWMData(int16_t Angle)
{//两个整数相除，结果也会是整数，小数部分会被直接丢弃，45 / 180=0.25直接为0

	Angle+=RF2_OFFSET;
	if(Angle > 180) Angle = 180;
	if(Angle < 0) Angle = 0;

	PWM_SetCompare2((float)(Angle)/180*(2500-500)+500);
	//0~180°对应CCR值500~2500区间设置
}
	
//更改CH3舵机角度
void Servo_SetAngle3_PWMData(int16_t Angle)
{//两个整数相除，结果也会是整数，小数部分会被直接丢弃，45 / 180=0.25直接为0

	Angle+=LB3_OFFSET;
	if(Angle > 180) Angle = 180;
	if(Angle < 0) Angle = 0;
	Angle=180-Angle;//解决舵机镜像偏移

	PWM_SetCompare3((float)(Angle)/180*(2500-500)+500);
	//0~180°对应CCR值500~2500区间设置
}
	
//更改CH4舵机角度
void Servo_SetAngle4_PWMData(int16_t Angle)
{//两个整数相除，结果也会是整数，小数部分会被直接丢弃，45 / 180=0.25直接为0

	Angle+=RB4_OFFSET;
	if(Angle > 180) Angle = 180;
	if(Angle < 0) Angle = 0;

	PWM_SetCompare4((float)(Angle)/180*(2500-500)+500);
	//0~180°对应CCR值500~2500区间设置
}

//更改CH5舵机角度
void Servo_SetAngle5_PWMData(int16_t Angle)
{//两个整数相除，结果也会是整数，小数部分会被直接丢弃，45 / 180=0.25直接为0
	Angle+=BH5_OFFSET;
	if(Angle > 180) Angle = 180;
	if(Angle < 0) Angle = 0;
	PWM_SetCompare5((float)Angle/180*(2500-500)+500);
	//0~180°对应CCR值500~2500区间设置
}

// ==========================================
//软件调用层，实现改目标数据功能，不动硬件
//单独修改某一个舵机的角度
void Servo_SetAngle1(uint16_t Angle)
	{Servo_TargetAngle[0]=Angle;}
void Servo_SetAngle2(uint16_t Angle)
	{Servo_TargetAngle[1]=Angle;}
void Servo_SetAngle3(uint16_t Angle)
	{Servo_TargetAngle[2]=Angle;}
void Servo_SetAngle4(uint16_t Angle)
	{Servo_TargetAngle[3]=Angle;}
void Servo_SetAngle5(uint16_t Angle)
	{Servo_TargetAngle[4]=Angle;}


///外层获取目标角度函数，五个舵机一起改
void Servo_SetAngleData(uint16_t Angle1,uint16_t Angle2,uint16_t Angle3,uint16_t Angle4,uint16_t Angle5)
{
	Servo_TargetAngle[0] = Angle1;
	Servo_TargetAngle[1] = Angle2;
	Servo_TargetAngle[2] = Angle3;
	Servo_TargetAngle[3] = Angle4;
	Servo_TargetAngle[4] = Angle5;
}

//判断单个舵机i是否运动完成
uint8_t Servo_IsMoving(uint8_t i)
{
	if(fabs(Servo_TargetAngle[i] - Servo_CurrtntAngle[i]) > 1.0)
	{
		return 1;//运行中
	}
	return 0;
}

//判断当前动作（全部舵机）是否完成
uint8_t Servo_AllIsMoving(void)
{
	for(uint8_t i=0;i<5;i++)
	{
		if(fabs(Servo_TargetAngle[i] - Servo_CurrtntAngle[i]) > 1.0)
		{
			return 1;//运行中，returm跳出函数
		}
	}
	return 0;//全完成返回0
}

// =================方案A：线性插值 (匀速运动)=========================
//设置舵机最终角度,TIM4使用丝滑变化
//要实现平滑运动，要把**“发指令”和“执行指令”**彻底分开。
//用 “全局变量传递指令”+“定时器中断异步执行”，让运动过程在后台自动完成，不占用主程序资源；
// void Servo_SetTargetAngle(void)
// {
// 	uint8_t Speed=2;
// 	for(uint8_t i=0;i<5;i++)
// 	{
// 		if(Servo_CurrtntAngle[i]<Servo_TargetAngle[i])
// 		{
// 			Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]+Speed;
// 			if(Servo_CurrtntAngle[i]>Servo_TargetAngle[i])
// 				{Servo_CurrtntAngle[i]=Servo_TargetAngle[i];}
// 		}else
// 		{
// 			Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]-Speed;
// 			if(Servo_CurrtntAngle[i]<Servo_TargetAngle[i])
// 				{Servo_CurrtntAngle[i]=Servo_TargetAngle[i];}
// 		}
// 		//把算好的当前角度写入硬件
// 		//外层函数实现硬件角度驱动，内层函数实现数据传输改变
// 		//实现指令（目标角度） 和 执行（慢慢挪）脱钩
// 		//不分开的话，中断里直接跳到目标，就失去了 “慢慢挪” 的过程，和 “丝滑” 的目标完全矛盾
// 		Servo_SetAngle1_PWMData(Servo_CurrtntAngle[0]);
// 		Servo_SetAngle2_PWMData(Servo_CurrtntAngle[1]);
// 		Servo_SetAngle3_PWMData(Servo_CurrtntAngle[2]);
// 		Servo_SetAngle4_PWMData(Servo_CurrtntAngle[3]);
// 		Servo_SetAngle5_PWMData(Servo_CurrtntAngle[4]);
// 	}
// }


// =================方案B：S型曲线 / 柔性控制（刚性两档调速，不推荐）=====================
//匀速运动基础上修改，在接近目标点时，降低速度，避免突停
// void Servo_SetTargetAngle(void)
// {
// 	uint8_t Speed=3;
// 	for(uint8_t i=0;i<5;i++)
// 	{
// 		if(Servo_CurrtntAngle[i]<Servo_TargetAngle[i])
// 		{
// 			if(fabs(Servo_TargetAngle[i] - Servo_CurrtntAngle[i])<40)
// 			{
// 				Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]+Speed-2;
// 				if(Servo_CurrtntAngle[i]>Servo_TargetAngle[i])
// 					{Servo_CurrtntAngle[i]=Servo_TargetAngle[i];}
// 			}else
// 			{
// 				Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]+Speed;
// 				if(Servo_CurrtntAngle[i]>Servo_TargetAngle[i])
// 					{Servo_CurrtntAngle[i]=Servo_TargetAngle[i];}
// 			}			
// 		}else
// 		{
// 			if(fabs(Servo_TargetAngle[i] - Servo_CurrtntAngle[i])<40)
// 			{
// 				Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]-Speed+2;
// 				if(Servo_CurrtntAngle[i]<Servo_TargetAngle[i])
// 					{Servo_CurrtntAngle[i]=Servo_TargetAngle[i];}
// 			}else
// 			{	
// 				Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]-Speed;
// 				if(Servo_CurrtntAngle[i]<Servo_TargetAngle[i])
// 					{Servo_CurrtntAngle[i]=Servo_TargetAngle[i];}
// 			}
// 		}
// 		//把算好的当前角度写入硬件
// 		//外层函数实现硬件角度驱动，内层函数实现数据传输改变
// 		//实现指令（目标角度） 和 执行（慢慢挪）脱钩
// 		//不分开的话，中断里直接跳到目标，就失去了 “慢慢挪” 的过程，和 “丝滑” 的目标完全矛盾
// 		Servo_SetAngle1_PWMData(Servo_CurrtntAngle[0]);
// 		Servo_SetAngle2_PWMData(Servo_CurrtntAngle[1]);
// 		Servo_SetAngle3_PWMData(Servo_CurrtntAngle[2]);
// 		Servo_SetAngle4_PWMData(Servo_CurrtntAngle[3]);
// 		Servo_SetAngle5_PWMData(Servo_CurrtntAngle[4]);
// 	}
// }


// ====================方案C：S型曲线 / 柔性控制（曲线调速）======================
void Servo_SetTargetAngle(void)
{
	
	//float	K = 0.1;// 比例系数,0.05~0.3之间。越大越快，越小越柔。
	//为实现外部调用更改速度，改为全局变量volatile float K ;
	float Speed=2;

	for(uint8_t i=0;i<5;i++)
	{
		//角度差
		float	DValue=fabs(Servo_TargetAngle[i] - Servo_CurrtntAngle[i]);

		if(DValue<0.5)
		{// 如果角度差很小，直接到位，防止抖动
			Servo_CurrtntAngle[i]=Servo_TargetAngle[i];
					
		}else
		{
			Speed= DValue * K;

			if(Servo_TargetAngle[i] < Servo_CurrtntAngle[i])
			{
				Speed=-Speed;
			}

			// 限制最小步长，不然最后会无限慢
			if(Speed<1 && Speed>0)		Speed=0.5;
			if(Speed>-1 && Speed<0)	Speed=-0.5;

			// 限制最大步长，防止电流过大炸机
			if(Speed > 3) 	Speed = 3;
			if(Speed < -3) 	Speed = -3;

			Servo_CurrtntAngle[i]=Servo_CurrtntAngle[i]+Speed;
		}
		
		//把算好的当前角度写入硬件
		//外层函数实现硬件角度驱动，内层函数实现数据传输改变
		//实现指令（目标角度） 和 执行（慢慢挪）脱钩
		//不分开的话，中断里直接跳到目标，就失去了 “慢慢挪” 的过程，和 “丝滑” 的目标完全矛盾
		// Servo_SetAngle1_PWMData(Servo_CurrtntAngle[0]);
		// Servo_SetAngle2_PWMData(Servo_CurrtntAngle[1]);

		// // //while((Servo_IsMoving(1) && Servo_IsMoving(2)) == 0);“A和B不同时为1”，就一直等待。
		// // while((Servo_IsMoving(0) || Servo_IsMoving(1)) == 1);//"A或B有一个为1"，就一直等待。
		
		// Servo_SetAngle3_PWMData(Servo_CurrtntAngle[2]);
		// Servo_SetAngle4_PWMData(Servo_CurrtntAngle[3]);
		// Servo_SetAngle5_PWMData(Servo_CurrtntAngle[4]);


		switch(i)
        {
            case 0: Servo_SetAngle1_PWMData((int16_t)Servo_CurrtntAngle[0]); break;
            case 1: Servo_SetAngle2_PWMData((int16_t)Servo_CurrtntAngle[1]); break;
            case 2: Servo_SetAngle3_PWMData((int16_t)Servo_CurrtntAngle[2]); break;
            case 3: Servo_SetAngle4_PWMData((int16_t)Servo_CurrtntAngle[3]); break;
            case 4: if(Servo_TailEnable) 
						Servo_SetAngle5_PWMData((int16_t)Servo_CurrtntAngle[4]); 
					break;
        }
	}
}


