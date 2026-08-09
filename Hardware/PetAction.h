#ifndef __PETACTION_H
#define __PETACTION_H

#include "stm32f10x.h"                  // Device header

extern volatile uint8_t PetAction_ActionState;
extern volatile uint8_t PetAction_NewDataFlag;

void PetAction_Init(void);
void PetAction_RecordCommand(void);
void Pet_ShowExpression(const uint8_t *Image);
void PetAction_SelectAction(void);


#endif
