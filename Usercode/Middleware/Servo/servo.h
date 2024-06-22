#ifndef __SERVO_H
#define __SERVO_H
#ifdef __cplusplus
extern "C" {
#endif
#include "usermain.h"
#include "tim.h"

extern uint8_t servo_flag[3];

/*************************************************************
 * 舵机板指令定义
 */
#define Servo_UART_HANDLE huart7

/*************************************************************
 * 舵机定义
 */
// 左侧前限位板
#define Left_Front_Seed_Timer_Handle  htim4
#define Left_Front_Seed_Timer_Channel TIM_CHANNEL_3
// 左侧推苗板
#define Left_Buffer_Seed_Timer_Handle  htim4
#define Left_Buffer_Seed_Timer_Channel TIM_CHANNEL_2

// 右侧前限位板
#define Right_Front_Seed_Timer_Handle  htim5
#define Right_Front_Seed_Timer_Channel TIM_CHANNEL_2
// 右侧推苗板
#define Right_Buffer_Seed_Timer_Handle  htim5
#define Right_Buffer_Seed_Timer_Channel TIM_CHANNEL_3

// 球推球板
#define Ball_Pull_Timer_Handle  htim15
#define Ball_Pull_Timer_Channel TIM_CHANNEL_1
// 球中夹爪
#define Ball_Middle_Timer_Handle  htim2
#define Ball_Middle_Timer_Channel TIM_CHANNEL_4
// 球左夹爪
#define Ball_Left_Timer_Handle  htim2
#define Ball_Left_Timer_Channel TIM_CHANNEL_3
// 球右夹爪
#define Ball_Right_Timer_Handle  htim2
#define Ball_Right_Timer_Channel TIM_CHANNEL_1

/*************************************************************
 * 舵机动作定义
 */

// 球动作
#define Ball_Servo_In()                                                                  \
    do {                                                                                 \
        __HAL_TIM_SET_COMPARE(&Ball_Left_Timer_Handle, Ball_Left_Timer_Channel, 750);    \
        __HAL_TIM_SET_COMPARE(&Ball_Right_Timer_Handle, Ball_Right_Timer_Channel, 2250); \
    } while (0) // 左右夹爪动作-----向内收缩

#define Ball_Servo_Out()                                                                 \
    do {                                                                                 \
        __HAL_TIM_SET_COMPARE(&Ball_Left_Timer_Handle, Ball_Left_Timer_Channel, 1750);   \
        __HAL_TIM_SET_COMPARE(&Ball_Right_Timer_Handle, Ball_Right_Timer_Channel, 1250); \
    } while (0) // 左右夹爪动作-----向外伸张

#define Ball_Servo_Grip()                                                                  \
    do {                                                                                   \
        __HAL_TIM_SET_COMPARE(&Ball_Middle_Timer_Handle, Ball_Middle_Timer_Channel, 1000); \
    } while (0) // 夹爪抓取动作

#define Ball_Servo_Reset()                                                                 \
    do {                                                                                   \
        __HAL_TIM_SET_COMPARE(&Ball_Middle_Timer_Handle, Ball_Middle_Timer_Channel, 2000); \
    } while (0) // 夹爪放松动作

#define Ball_Servo_Ready()                                                             \
    do {                                                                               \
        __HAL_TIM_SET_COMPARE(&Ball_Pull_Timer_Handle, Ball_Pull_Timer_Channel, 1900); \
    } while (0) // 射球准备动作

#define Ball_Servo_Fire()                                                              \
    do {                                                                               \
        __HAL_TIM_SET_COMPARE(&Ball_Pull_Timer_Handle, Ball_Pull_Timer_Channel, 1400); \
    } while (0) // 射球动作

// 苗动作
#define Seed_Grip()        \
    do {                   \
        servo_flag[0] = 1; \
    } while (0) // 夹爪夹取苗动作

#define Seed_Deposit()     \
    do {                   \
        servo_flag[0] = 0; \
    } while (0) // 夹爪放置苗动作

#define Seed_Deposit_Buffer_Open()                                                                     \
    do {                                                                                               \
        __HAL_TIM_SET_COMPARE(&Left_Buffer_Seed_Timer_Handle, Left_Buffer_Seed_Timer_Channel, 2300);   \
        __HAL_TIM_SET_COMPARE(&Right_Buffer_Seed_Timer_Handle, Right_Buffer_Seed_Timer_Channel, 1100); \
    } while (0) // 推苗板打开

#define Seed_Deposit_Buffer_Close()                                                                    \
    do {                                                                                               \
        __HAL_TIM_SET_COMPARE(&Left_Buffer_Seed_Timer_Handle, Left_Buffer_Seed_Timer_Channel, 1700);   \
        __HAL_TIM_SET_COMPARE(&Right_Buffer_Seed_Timer_Handle, Right_Buffer_Seed_Timer_Channel, 1400); \
    } while (0) // 推苗板放下

#define Seed_Plant_Open()  \
    do {                   \
        servo_flag[1] = 1; \
    } while (0) // 放苗板打开

#define Seed_Plant_Close() \
    do {                   \
        servo_flag[1] = 0; \
    } while (0) // 放苗板关闭

#define Seed_Deposit_Close() \
    do {                     \
        servo_flag[2] = 1;   \
    } while (0) // 轨道限位板关闭

#define Seed_Deposit_Open() \
    do {                    \
        servo_flag[2] = 0;  \
    } while (0) // 轨道限位板打开

#define Seed_Front_Close()                                                                           \
    do {                                                                                             \
        __HAL_TIM_SET_COMPARE(&Left_Front_Seed_Timer_Handle, Left_Front_Seed_Timer_Channel, 1500);   \
        __HAL_TIM_SET_COMPARE(&Right_Front_Seed_Timer_Handle, Right_Front_Seed_Timer_Channel, 1500); \
    } while (0) // 前侧限位板关闭

#define Seed_Front_Open()                                                                           \
    do {                                                                                            \
        __HAL_TIM_SET_COMPARE(&Left_Front_Seed_Timer_Handle, Left_Front_Seed_Timer_Channel, 2400);  \
        __HAL_TIM_SET_COMPARE(&Right_Front_Seed_Timer_Handle, Right_Front_Seed_Timer_Channel, 600); \
    } while (0) // 前侧限位板打开

/*************************************************************
 * 用户函数
 */

void m_Servo_Init(void);
void m_Servo_TaskStart(void);
void m_Servo_Task(void *argument);

#ifdef __cplusplus
}
#endif
#endif