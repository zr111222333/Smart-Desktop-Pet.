#include "stm32f10x.h"
#include "PETACTION.h"

volatile uint8_t BlueTooth_RxData;
volatile uint8_t BlueTooth_RxFlag; 
void BlueTooth_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//推挽复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入或带上拉输入
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate=115200; // 蓝牙默认波特率通常是115200;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//对通信可靠性要求极高的防止因收发双方速度不匹配而导致的数据丢失
    USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity=USART_Parity_No;//奇偶校验为
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART3,&USART_InitStructure);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//接收中断开启
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;//代表 USART3 外设的中断号编号
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;// 优先级稍微比语音模块串口低一点
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART3,ENABLE);
}

//串口发送
void BlueTooth_SendData(uint8_t Byte)
{
    USART_SendData(USART3,Byte);//硬件会自动截取低 8 位发送
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);//判断是否发送完成   
}

//串口接收
// uint8_t BlueTooth_ReceiveData(void)
// {//不用中断接收使用此函数，CPU不断轮询降效
//     uint8_t Temp;
//     Temp=USART_ReceiveData(USART3);
//     while(USART_GetFlagStatus(USART3,USART_FLAG_RXNE));
//     return Temp;
// }
uint8_t BlueTooth_ReceiveData(void)
{
    return BlueTooth_RxData;//返回中断接收到的数据
}

//
uint8_t BlueTooth_GetRxFlag(void)
{
    if(BlueTooth_RxFlag==1)
    {
        BlueTooth_RxFlag=0;//读取后清除标志位
        return 1;// 告诉主程序有新数据
    }
    return 0;// 无新数据
}

void USART3_IRQHandler(void)//中断只负责收数据和通知
{
    if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)//数据接收中断开启
    {
        BlueTooth_RxData=USART_ReceiveData(USART3);//中断传递接收的数据
        BlueTooth_RxFlag=1;//接收到数据标志位置1,用于判断有无新数据              
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);//中断模式下，要清除中断挂起位，否则卡死在中断程序
    }
}
