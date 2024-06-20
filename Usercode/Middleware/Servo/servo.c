#include "servo.h"

char seed_grip_msg[62]        = {"{#000P1550T0020!#023P1500T0020!#001P1450T0020!#022P1500T0020!}"};
char seed_deposit_msg[62]     = {"{#000P1300T0020!#023P1700T0020!#001P1600T0020!#022P1300T0020!}"};
char seed_plant_open_msg[32]  = {"{#010P1150T0020!#013P1850T0020!}"};
char seed_plant_close_msg[32] = {"{#010P1500T0020!#013P1500T0020!}"};
char seed_buffer_close_msg[32] = {"{#011P1600T0020!#012P1600T0020!}"};
char seed_buffer_open_msg[32] = {"{#011P0600T0020!#012P2400T0020!}"};

/**
 * @brief 舵机初始化
 */
void m_Servo_Init(void)
{
    // 左侧舵机
    HAL_TIM_PWM_Start(&Left_Front_Seed_Timer_Handle, Left_Front_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Left_Buffer_Seed_Timer_Handle, Left_Buffer_Seed_Timer_Channel);
    // 右侧舵机
    HAL_TIM_PWM_Start(&Right_Front_Seed_Timer_Handle, Right_Front_Seed_Timer_Channel);
    HAL_TIM_PWM_Start(&Right_Buffer_Seed_Timer_Handle, Right_Buffer_Seed_Timer_Channel);
    // 球舵机
    HAL_TIM_PWM_Start(&Ball_Pull_Timer_Handle, Ball_Pull_Timer_Channel);
    HAL_TIM_PWM_Start(&Ball_Middle_Timer_Handle, Ball_Middle_Timer_Channel);
    HAL_TIM_PWM_Start(&Ball_Left_Timer_Handle, Ball_Left_Timer_Channel);
    HAL_TIM_PWM_Start(&Ball_Right_Timer_Handle, Ball_Right_Timer_Channel);

    Ball_Servo_In();
    Ball_Servo_Grip();
}