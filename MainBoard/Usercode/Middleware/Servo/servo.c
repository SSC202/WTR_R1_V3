#include "servo.h"

uint8_t servo_flag[3] = {0, 0, 0};

osThreadId_t servo_uart_TaskHandle;
const osThreadAttr_t servo_uart_Task_attributes = {
    .name       = "servo_uart_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

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

/**
 * @brief   舵机线程开启
 */
void m_Servo_TaskStart(void)
{
    servo_uart_TaskHandle = osThreadNew(m_Servo_Task, NULL, &servo_uart_Task_attributes);
}

/**
 * @brief   舵机线程
 */
void m_Servo_Task(void *argument)
{
    static uint8_t send_buffer[7];
    send_buffer[0] = 0xAA;
    send_buffer[1] = 0xAA;
    send_buffer[5] = 0xBB;
    send_buffer[6] = 0xBB;
    for (;;) {
        send_buffer[2] = servo_flag[0];
        send_buffer[3] = servo_flag[1];
        send_buffer[4] = servo_flag[2];
        HAL_UART_Transmit(&Servo_UART_HANDLE, send_buffer, 7, 0xFF);
        osDelay(1);
    }
}
