#include "stm32f10x.h"                  // Device header
#include "SERVO.H"
#include "Action.h"
#include "BreathLED.h"
#include "PetAction.h"

uint16_t i=0;
volatile uint16_t TimeCount=0;


//产生20ms的中断，用于改变舵机变化使更加丝滑
void  Time4_Init(void)
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能TIM4时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB6 PB7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    TIM_InternalClockConfig(TIM4); //配置TIM4的内部时钟模式


    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//只要 APB1 预分频不是 1，定时器时钟就会自动 ×2,所以也是72MHz
    TIM_TimeBaseInitStructure.TIM_Period=20000-1; //计数周期为20000，即1ms
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//从0开始
    TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;// 重复计数器：0（仅高级定时器需要）
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);

    // 配置NVIC中断控制器
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //使能更新计数溢出中断
    
    TIM_Cmd(TIM4, ENABLE); //使能TIM4
}


void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)//如果溢出中断发生
    {
        TimeCount++;//20ms计数加1               
        BreathLED_UpdateLighTtime();//呼吸灯开关更新
        PetAction_RecordCommand();//语音模块动作数据采集
        
        Servo_SetTargetAngle(); //执行舵机运动
        if(TimeCount>=65535) TimeCount=0;//防止溢出
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清除标志位
    }

}
