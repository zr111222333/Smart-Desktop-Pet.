 #include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	//开启舵机时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//TIM2,36MHz，脚
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//TIM3,36MHz,尾巴
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIOA
	
	//GPIOB - 呼吸灯
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//舵机引脚初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//参考手册规定8.1.11片上外设定时器2输出模式用复用推挽模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//呼吸灯引脚初始化
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 ;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	

	//定时器初始化
	TIM_InternalClockConfig(TIM2);//配置TIM2的内部时钟模式
	TIM_InternalClockConfig(TIM3);
	
	//脚，20ms周期
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//只要 APB1 预分频不是 1，定时器时钟就会自动 ×2,所以也是72MHz
	//这是 STM32 的硬件设计规则（目的是让低速总线的定时器也能获得高频时钟，满足高精度定时需求）。
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//从0开始
	TIM_TimeBaseStructure.TIM_Period=20000-1;
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;//预分频器值PSC
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;// 重复计数器：0（仅高级定时器需要）
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	//脚
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);////给结构体赋予默认的初始值，防止要用高级定时器是用上下面通用定时器用的时候没赋值的参数，导致值不确定出bug
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//该模式CCR 值越大，高电平时长越长
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//决定 “有效电平是高电平（High）还是低电平（Low）
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;//CCR PWM 高电平时长 / 占空比	
	//TIM2_PA0_CH1
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	// 使能预装载,新写入的CCR值不会立即生效,等正在执行周期过后写入，防止舵机抖动、角度识别错误
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  
	
	
	//尾巴
	TIM_OCStructInit(&TIM_OCInitStructure);////给结构体赋予默认的初始值，防止要用高级定时器是用上下面通用定时器用的时候没赋值的参数，导致值不确定出bug
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//该模式CCR 值越大，高电平时长越长
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//决定 “有效电平是高电平（High）还是低电平（Low）
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;//CCR PWM 高电平时长 / 占空比
	
	//TIM3_PA6_CH1
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	// 使能预装载,新写入的CCR值不会立即生效,等正在执行周期过后写入，防止舵机抖动、角度识别错误
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 

	//舵机
	//TIM2_PA0_CH1
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);    // 使能预装载	
	
	//TIM2_PA1_CH2
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);    // 使能预装载
	
	//TIM2_PA2_CH3
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);    // 使能预装载
	
	//TIM2_PA3_CH4
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);    // 使能预装载
	
	//TIM3_PA6_CH1
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);    // 使能预装载

	//呼吸灯
	//TIM3_PB0_CH3
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);    // 使能预装载
	
	//TIM3_PB1_CH4
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);    // 使能预装载
	
	TIM_Cmd(TIM2,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
}


//改CH1的ARR，占空比
void PWM_SetCompare1(uint16_t Compare)//周期为20us，但是cnt是o到20000.所以是uint16,8不合适
{
	TIM_SetCompare1(TIM2,Compare);//比较值，占空比的数
}

//改CH2的ARR，占空比
void PWM_SetCompare2(uint16_t Compare)//周期为20us，但是cnt是o到20000.所以是uint16,8不合适
{
	TIM_SetCompare2(TIM2,Compare);//比较值，占空比的数
}

//改CH3的ARR，占空比
void PWM_SetCompare3(uint16_t Compare)//周期为20us，但是cnt是o到20000.所以是uint16,8不合适
{
	TIM_SetCompare3(TIM2,Compare);//比较值，占空比的数
}

//改CH4的ARR，占空比
void PWM_SetCompare4(uint16_t Compare)//周期为20us，但是cnt是o到20000.所以是uint16,8不合适
{
	TIM_SetCompare4(TIM2,Compare);//比较值，占空比的数
}

//尾巴，改CH1的ARR，占空比
void PWM_SetCompare5(uint16_t Compare)//周期为20us，但是cnt是o到20000.所以是uint16,8不合适
{
	TIM_SetCompare1(TIM3,Compare);//比较值，占空比的数
}


//呼吸灯，改CH3的ARR，占空比
void PWM_SetCompare6(uint16_t Compare)//周期为20us，但是cnt是o到20000.所以是uint16,8不合适
{
	TIM_SetCompare3(TIM3,Compare);//比较值，占空比的数
	TIM_SetCompare4(TIM3,Compare);//比较值，占空比的数	
}


