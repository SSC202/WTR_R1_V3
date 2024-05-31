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
        speedServo(v_1, &hDJI[0][0]);
        speedServo(v_2, &hDJI[1][0]);
        speedServo(v_3, &hDJI[2][0]);
        speedServo(v_4, &hDJI[3][0]);
        positionServo(arm_angle, &hDJI[4][0]);
        speedServo(-friction_speed_up, &hDJI[6][0]);
        speedServo(friction_speed_down, &hDJI[7][0]);
        positionServo(motor_r_gripseed, &hDJI[0][1]);
        positionServo(motor_l_gripseed, &hDJI[1][1]);
        positionServo(motor_r_plantseed, &hDJI[2][1]);
        positionServo(motor_l_plantseed, &hDJI[3][1]);
        CanTransmit_DJI_1234(fdcan1, hDJI[0][0].speedPID.output, hDJI[1][0].speedPID.output, hDJI[2][0].speedPID.output, hDJI[3][0].speedPID.output);
        if (pick_flag == 1) {
            CanTransmit_DJI_5678(fdcan1, hDJI[4][0].speedPID.output - 800, 0, hDJI[6][0].speedPID.output, hDJI[7][0].speedPID.output);
        } else {
            CanTransmit_DJI_5678(fdcan1, hDJI[4][0].speedPID.output, 0, hDJI[6][0].speedPID.output, hDJI[7][0].speedPID.output);
        }
        CanTransmit_DJI_1234(fdcan2, hDJI[0][1].speedPID.output, hDJI[1][1].speedPID.output, hDJI[2][1].speedPID.output, hDJI[3][1].speedPID.output);
        osDelay(1);
    }
}

/**
 * @brief   底盘电机控制线程
 */
void m_Chassis_Ctl_Task(void *argument)
{
    static float mvx;
    static float mvy;
    static float mwc;
    for (;;) {
        if (run_state == HANDLE_MODE) {
            // 手动模式下为遥控控制底盘
            mvx = (float)(usr_left_y * 200) / 4000.0;
            mvy = -(float)(usr_left_x * 200) / 4000.0;
        } else if (run_state == AUTO_MODE) {
            // TODO：接收自动线程接口传回的消息
            mvx = 0;
            mvy = 0;
            mwc = 0;
        }
        Inverse_kinematic_equation(mvx, mvy, mwc, &v_1, &v_2, &v_3, &v_4);
        osDelay(2);
    }
}

/**********************************底盘运动学解算相关函数*************************************/

/**
 * @brief   逆运动学方程，输入速度求解电机转速
 * @param   vx          底盘前向速度，单位m/s
 * @param   vy          底盘左横向速度，单位m/s
 * @param   wc          转向角速度，使用弧度，单位rad/s
 */
void Inverse_kinematic_equation(float vx, float vy, float wc, float *_v_1, float *_v_2, float *_v_3, float *_v_4)
{
    float v1, v2, v3, v4;
    v1 = (float)((sqrt(2) * vx / 2 - sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);
    v2 = (float)((-sqrt(2) * vx / 2 - sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);
    v3 = (float)((-sqrt(2) * vx / 2 + sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);
    v4 = (float)((sqrt(2) * vx / 2 + sqrt(2) * vy / 2 - wc * chassis_r) / chassis_R);

    *_v_1 = (float)(v1 * 60.0) / (2 * PI);
    *_v_2 = (float)(v2 * 60.0) / (2 * PI);
    *_v_3 = (float)(v3 * 60.0) / (2 * PI);
    *_v_4 = (float)(v4 * 60.0) / (2 * PI);
}