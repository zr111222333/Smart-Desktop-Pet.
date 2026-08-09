#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f10x.h"                  // Device header

extern uint16_t Angle;
extern float Servo_CurrtntAngle[];//当前角度
extern int16_t Servo_TargetAngle[];//目标角度
extern volatile float K;
extern volatile  uint8_t Servo_TailEnable;

void Servo_Init(void);

//底层硬件调用层，实现改PWM数据功能，动硬件
void Servo_SetAngle1_PWMData(int16_t Angle);
void Servo_SetAngle2_PWMData(int16_t Angle);
void Servo_SetAngle3_PWMData(int16_t Angle);
void Servo_SetAngle4_PWMData(int16_t Angle);
void Servo_SetAngle5_PWMData(int16_t Angle);

//软件调用层，实现改目标数据功能，不动硬件
//单独修改某一个舵机的角度
void Servo_SetAngle1(uint16_t Angle);
void Servo_SetAngle2(uint16_t Angle);
void Servo_SetAngle3(uint16_t Angle);
void Servo_SetAngle4(uint16_t Angle);
void Servo_SetAngle5(uint16_t Angle);

//外层获取目标角度函数
//同时修改5舵机目标数据角度，外层调用的
void Servo_SetAngleData(uint16_t Angle1,uint16_t Angle2,uint16_t Angle3,uint16_t Angle4,uint16_t Angle5);

uint8_t Servo_IsMoving(uint8_t i);
uint8_t Servo_AllIsMoving(void);

//最终实现舵机丝滑运动的函数,内层定时器中断里执行的
void Servo_SetTargetAngle(void);

#endif
