#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "TIME4.h"

volatile uint8_t BreathLED_LEDState=1;

//法一：基于时间的开环控制
//程序可能因为某种原因（比如中断干扰、逻辑修改）导致 CNT 计数错乱，或者加减次数不对等，在CNT归零时可能并不是0。
//可能导致亮度会“漂移”，甚至导致变量溢出（Overflow），呼吸灯会出现闪烁或乱码现象。
//且修改参数需重新计算数学关系不方便
// void BreathLED_UpdateLighTtime(void)
// {
//     static uint16_t CNT=0;
//     CNT++;
//    static uint16_t BreathLED_DutyCycle=0;
//   if(CNT<=100)
//   {
//     BreathLED_DutyCycle=BreathLED_DutyCycle+200;
//     PWM_SetCompare6(BreathLED_DutyCycle); 
//   }
//   else if(CNT>100 && CNT<=200)
//   {
//     BreathLED_DutyCycle=BreathLED_DutyCycle-200;
//     PWM_SetCompare6(BreathLED_DutyCycle); 
//   }
//   else if(CNT>200)
//   {
//     CNT=0;
//   }
// }

//法二：基于状态的闭环控制
//无论中间发生了什么跳变，只要触碰到边界，数值就会被强制校正回安全范围。
//它永远不会溢出，也不会漂移，而且修改参数也非常方便。
void BreathLED_UpdateLighTtime()
{
    static uint16_t BreathLED_DutyCycle = 0;     // 当前占空比 0~20000
    static uint8_t BreathLED_Direction = 1;      // 1=变亮, 0=变暗   
    if(BreathLED_LEDState==1) // 呼吸灯开
    {        
        if(BreathLED_Direction) // 变亮
        {
            BreathLED_DutyCycle += 200;  // 每20ms增加200
            if(BreathLED_DutyCycle >= 20000)
            {
                BreathLED_DutyCycle = 20000;
                BreathLED_Direction = 0; // 切换为变暗
            }
        }
        else // 变暗
        {
            if(BreathLED_DutyCycle >= 200)
                BreathLED_DutyCycle -= 200;
            else
            {
                BreathLED_DutyCycle = 0;
                BreathLED_Direction = 1; // 切换为变亮
            }
        }
        // 更新PWM占空比
        PWM_SetCompare6(BreathLED_DutyCycle);
    }
    else if(BreathLED_LEDState==0)// 呼吸灯关
    {
        PWM_SetCompare6(20000); // 常亮
    }
    else if(BreathLED_LEDState==2) //关灯
    {
        PWM_SetCompare6(0);
    }
}





