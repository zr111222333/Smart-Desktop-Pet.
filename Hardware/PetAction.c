#include "stm32f10x.h"                  // Device header
#include "Action.h"
#include "Servo.h"
#include "Serial.h"
#include "BlueTooth.h"
#include "OLED.h"
#include "Time4.h"

volatile uint8_t PetAction_ActionState=0;
volatile uint8_t PetAction_NewDataFlag=0;  // 新数据标志位
uint8_t IsVoltageMode = 0;//电压显示模式开关 0: 正常显示表情。1: 开启电压显示（屏蔽表情）
void PetAction_Init(void)
{
    Serial_Init();
    Servo_Init();
    BlueTooth_Init();
    //发送目标角度数据
    Servo_SetAngleData(0,0,0,0,0);
    // //实现目标角度操作
    // Servo_SetTargetAngle();
}

// 在中断里只调用这个函数，仅记录命令
void PetAction_RecordCommand(void)
{ 
        // uint8_t PetAction_ActionSNtateNew=0;
        // PetAction_ActionSNtateNew = Serial_ReceiveData();
        // if(PetAction_ActionSNtateNew != PetAction_ActionState)
        // {
        //     PetAction_ActionState = PetAction_ActionSNtateNew;
        // }else
        // {
        //     PetAction_ActionState=0; // 如果没有新数据，保持为0
        // }
    if(Serial_GetRxFlag()== 1 )
    {        
        PetAction_ActionState = Serial_ReceiveData();
        PetAction_NewDataFlag = 1;  // 标记有新数据
    }
    if(BlueTooth_GetRxFlag() == 1 )
    {
        PetAction_ActionState = BlueTooth_ReceiveData();
        PetAction_NewDataFlag = 1;  // 标记有新数据
    }
}


// 表情显示清理刷新
void Pet_ShowExpression(const uint8_t *Image)
{
    if (IsVoltageMode == 1)
    {
        return; // 如果处于电压显示模式，跳过表情显示
    }
    OLED_Clear(); // 先清屏，防止之前的残留
    OLED_ShowImage(0, 0, 128, 64, Image); // 显示 128x64 的图片
    OLED_Update(); // 更新显存到屏幕
}

//接收串口数据判断动作指令
void PetAction_SelectAction(void)
{ 
    //不管有没有新数据ADC值一直读ADC值显示电压值
    static uint16_t Last_TimeCount = 0;
    if(IsVoltageMode == 1)//进入电压显示模式
    {
        // TimeCount 是20ms中断一次
        // 这里设置差值 >= 25，意味着 25 * 20ms = 500ms 刷新一次
        // 这样可以避免屏幕闪烁，也不会因为频繁刷屏卡死动作
        if(TimeCount >= Last_TimeCount + 25 || TimeCount < Last_TimeCount) 
        {
            Action_ShowVoltage(); // 更新电压数值
            Last_TimeCount = TimeCount; // 更新记录时间
        }
    }
    // 接收到新数据，执行对应动作
    if(PetAction_ActionState )
    {
        uint8_t Command = PetAction_ActionState;
        PetAction_ActionState = 0; // 清除指令状态，以便在动作执行期间接收新指令       
        switch(Command)
        {
            case 0x29:Action_TailWagStop();break;
            case 0x30:Pet_ShowExpression(Face_stare);Action_SitDown();break;
            case 0x31:Pet_ShowExpression(Face_stare);Action_Stand();break;
            case 0x32:Pet_ShowExpression(Face_sleep);Action_LieDown();break;
            case 0x33:Pet_ShowExpression(Face_happy);Action_Run();break;
            case 0x34:Pet_ShowExpression(Face_stare);Action_Back();break;
            case 0x35:Pet_ShowExpression(Face_hello);Action_TurnLeft();break;
            case 0x36:Pet_ShowExpression(Face_hello);Action_TurnRight();break;
            case 0x37:Pet_ShowExpression(Face_mania);Action_Shake();break;
            case 0x38:Pet_ShowExpression(Face_happy);Action_RunFast();break;
            case 0x39:Pet_ShowExpression(Face_mania);Action_ShakeFast();break;
            case 0x40:Pet_ShowExpression(Face_eyes);Action_TailWag();break;
            case 0x41:Pet_ShowExpression(Face_very_happy);Action_JumpForward();break;
            case 0x42:Pet_ShowExpression(Face_very_happy);Action_JumpBackward();break;
            case 0x43:Pet_ShowExpression(Face_hello);Action_Hello();break;
            case 0x44:Action_AllLEDON();break;
            case 0x45:Action_AllLEDOFF();break;
            case 0x46:Action_LightON();break;
            case 0x47:Action_LightOFF();break;
            case 0x48:Pet_ShowExpression(Face_eyes);Action_Stretch();break;
            case 0x49:Pet_ShowExpression(Face_eyes);Action_LegStretch();break;
            case 0x50: OLED_Clear();
                    if(IsVoltageMode == 0)// 如果处于正常显示模式，切换到电压显示模式
                    {                      
                        IsVoltageMode = 1;
                        OLED_Clear();        
                        Action_ShowVoltage(); 
                    }
                    else// 如果当前是电压模式，切换回普通模式
                    {              
                        IsVoltageMode = 0;                 
                        Pet_ShowExpression(Face_stare); // 恢复默认表情
                    }
                    break;
            case 0x51: OLED_Clear();Action_SerialTest();break;
            default:break;     
        } 
    }
}

