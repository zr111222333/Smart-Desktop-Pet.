#include "stm32f10x.h"                  // Device header

uint8_t Serial_RxData;//接收数据参数
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//一次GPIO_Init调用只能初始化一组相同配置的引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件流控
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//接收中断开启
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//开关指定的中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}


//发数据
void Serial_SendData(uint8_t Byte)
{
	USART_SendData(USART1,Byte);//硬件会自动截取低 8 位发送
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//CPU 写入数据的速度远快于串口发送的速度
}

//接收数据
uint8_t Serial_ReceiveData(void)
{
	
	return Serial_RxData;
}

//判断是否有新数据,少了无法判断中断接收是否完成
uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag==0)//没新数据回0
	{
		return 0;
	}else
	{
		Serial_RxFlag=0;//标志位清零，一旦读取成功，立马清零
		return 1;
	}
}


//中断函数
void USART1_IRQHandler(void)//单片机接收语音识别模块发送对应的数据来产生动作
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)//中断函数内用中断相关判断，在共享同一个中断处理函数的多个中断事件中，精确地识别出当前发生的是哪一个
	{
		Serial_RxData=USART_ReceiveData(USART1);
		Serial_RxFlag=1;
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//中断模式下，仅清标志位≠清中断，要清除中断挂起位，否则卡死在中断程序
	}
	
}



