#ifndef __CHASSIS_H
#define __CHASSIS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "usermain.h"
#include "Caculate.h"
#include "math.h"
#include "wtr_can.h"

/*************************************************************
 * 用户函数
*/

void m_Chassis_CAN_Message_TaskStart(void);
void m_Chassis_Init(void);

#ifdef __cplusplus
}
#endif
#endif