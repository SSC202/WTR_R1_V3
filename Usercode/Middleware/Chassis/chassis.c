#include "chassis.h"

/****************************************************************************
 * 线程定义
 */

osThreadId_t can_message_TaskHandle;
const osThreadAttr_t can_message_Task_attributes = {
    .name       = "can_message_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityHigh1,
};
void m_CAN_Message_Task(void *argument);

/****************************************************************************
 * 电机初始化 CAN消息发送相关函数
 */

/**
 * @brief   底盘初始化
 */
void m_Chassis_Init(void)
{
    fdcan1.CAN_Rx_Filter_Init();
    fdcan2.CAN_Rx_Filter_Init();
    fdcan1.CAN_Start();
    fdcan2.CAN_Start();
    fdcan1.CAN_Interrupt_Init();
    fdcan2.CAN_Interrupt_Init();
    hDJI[0][0].motorType = M3508;
    hDJI[1][0].motorType = M3508;
    hDJI[2][0].motorType = M3508;
    hDJI[3][0].motorType = M3508;
    hDJI[4][0].motorType = M3508;
    hDJI[5][0].motorType = M3508;
    hDJI[6][0].motorType = M3508;
    hDJI[0][1].motorType = M2006; // 右侧取苗爪升降电机
    hDJI[1][1].motorType = M2006; // 右侧存储机构运动电机
    hDJI[2][1].motorType = M2006; // 左侧取苗爪升降电机
    hDJI[3][1].motorType = M2006; // 左侧存储机构运动电机
    DJI_Init();
}
/**
 * @brief   底盘电机CAN消息发送线程创建
 */
void m_Chassis_CAN_Message_TaskStart(void)
{
    can_message_TaskHandle = osThreadNew(m_CAN_Message_Task, NULL, &can_message_Task_attributes);
}

/****************************************************************************
 * 线程函数
 */

/**
 * @brief   底盘电机CAN消息发送线程
 */
void m_CAN_Message_Task(void *argument)
{
    for (;;) {
        
        osDelay(1);
    }
}
