#ifndef __ACTION_H
#define __ACTION_H

#include "stm32f10x.h"                  // Device header


void Action_Init(void);
void Action_WaitUntilMovingComplete(void);
void Action_Stand(void);
void Action_LieDown(void);
void Action_SitDown(void);
void Action_Run(void);
void Action_Back(void);
void Action_TurnLeft(void);
void Action_TurnRight(void);
void Action_Shake(void);
void Action_RunFast(void);
void Action_ShakeFast(void);
void Action_TailWag(void);
void Action_TailWagStop(void);
void Action_JumpForward(void);
void Action_JumpBackward(void);
void Action_Hello(void);    
void Action_Stretch(void);
void Action_LegStretch(void);
void Action_LightON(void);
void Action_LightOFF(void);
void Action_AllLEDON(void);
void Action_AllLEDOFF(void);
void Action_ShowVoltage(void);
void Action_SerialTest(void);

#endif
