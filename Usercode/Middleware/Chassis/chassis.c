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

osThreadId_t chassis_ctl_TaskHandle;
const osThreadAttr_t chassis_ctl_Task_attributes = {
    .name       = "chassis_ctl_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_Chassis_Ctl_Task(void *argument);
/****************************************************************************
 * 接口定义
 */

int arm_angle           = 0; // 机械臂角度
int friction_speed_up   = 0; // 上摩擦轮速度
int friction_speed_down = 0; // 下摩擦轮速度
float v_1, v_2, v_3, v_4;    // 底盘速度
float motor_r_gripseed;      // 右侧取苗电机位置
float motor_l_gripseed;      // 左侧取苗电机位置
float motor_r_plantseed;     // 右侧放苗电机位置
float motor_l_plantseed;     // 左侧放苗电机位置

/****************************************************************************
 * 电机初始化 CAN消息发送相关函数
 */

/**
 * @brief   底盘初始化
 */
void m_Chassis_Init(void)
{
    fdcan1.FDCAN_Rx_Filter_Init();
    fdcan2.FDCAN_Rx_Filter_Init();
    fdcan1.FDCAN_Start();
    fdcan2.FDCAN_Start();
    fdcan1.FDCAN_Interrupt_Init();
    fdcan2.FDCAN_Interrupt_Init();
    hDJI[0][0].motorType = M3508;
    hDJI[1][0].motorType = M3508;
    hDJI[2][0].motorType = M3508;
    hDJI[3][0].motorType = M3508;
    hDJI[4][0].motorType = M3508;
    hDJI[6][0].motorType = M3508;
    hDJI[7][0].motorType = M3508;
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

/**
 * @brief   底盘控制启动线程
 */
void m_Chassis_Ctl_TaskStart(void)
{
    chassis_ctl_TaskHandle = osThreadNew(m_Chassis_Ctl_Task, NULL, &chassis_ctl_Task_attributes);
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
        CanTransmit_DJI_1234(fdcan1, 0, 0, 0, 0);
        CanTransmit_DJI_5678(fdcan1, 0, 0, 0, 0);
        CanTransmit_DJI_1234(fdcan2, 0, 0, 0, 0);
        osDelay(1);
    }
}
