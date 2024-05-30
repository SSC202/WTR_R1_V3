#include "handle_ball_task.h"

enum Handle_Ball_State handle_ball_state = Handle_Ball_Ready;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t handle_ball_TaskHandle;
const osThreadAttr_t handle_ball_Task_attributes = {
    .name       = "handle_ball_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_handle_ball_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   手动取放球线程初始化
 */
void m_handle_ball_Task_Start(void)
{
    handle_ball_state      = Handle_Ball_Ready;
    handle_ball_TaskHandle = osThreadNew(m_handle_ball_Task, NULL, &handle_ball_Task_attributes);
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放球线程
 */
void m_handle_ball_Task(void *argument)
{
    for (;;) {
        switch (handle_ball_state) {
            case Handle_Ball_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_ready_msg, &mav_handle_ball_msg);
                // 复位状态
                // 1. 球复位状态
                /***************************************
                 * 默认动作
                 *      1. 左右夹爪内收
                 *      2. 中夹爪夹取状态
                 *      3. 供球板打开
                 *      4. 摩擦轮停止
                 *      5. 机械臂内收
                 */
                Ball_Servo_Grip();
                Ball_Servo_In();
                Ball_Servo_Ready();
                arm_angle           = -160;
                friction_speed_up   = 0;
                friction_speed_down = 0;
                while (((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                       ((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                    osDelay(1);
                }
                // 2. 苗复位状态
                /*******************************************
                 * 默认动作
                 *      1. 宇树电机内收；
                 *      2. 夹取状态；
                 *      3. 取苗门关闭；
                 *      4. 放苗门关闭；
                 *      5. 限位后退至末端；
                 *      6. 电机处于高位;
                 *      7. 前侧限位板关闭
                 */
                unitree_right_pos = PI / 4;
                unitree_left_pos  = -PI / 4;
                Seed_Grip();
                Seed_Deposit_Close();
                Seed_Plant_Close();
                Seed_Deposit_Buffer_Open();
                Seed_Front_Close();
                motor_l_gripseed = -695;
                motor_r_gripseed = -695;
                while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
                       ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
                       ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
                       ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
                    osDelay(1);
                }
                motor_l_plantseed = 0;
                motor_r_plantseed = 0;
                while (((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                       ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                       ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                       ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                    osDelay(1);
                }
                // 状态机转换
                if (btn_KnobR == 1) {
                    handle_ball_state = Handle_Ball_Pick;
                }
                break;
            case Handle_Ball_Pick:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_pick_msg, &mav_handle_ball_msg);
                // 动作指令
                Ball_Servo_Out();
                osDelay(5);
                Ball_Servo_Reset();
                osDelay(5);
                arm_angle = -5;
                while (((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                       ((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                    osDelay(1);
                }
                // 状态转换
                handle_ball_state = Handle_Ball_Fire;
                break;
            case Handle_Ball_Fire:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_fire_msg, &mav_handle_ball_msg);
                // 动作指令
                friction_speed_down = 100;
                friction_speed_up   = -100;
                osDelay(2000);
                Ball_Servo_Grip();
                osDelay(500);
                Ball_Servo_In();
                arm_angle = -160;
                while (((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                       ((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                    osDelay(1);
                }
                osDelay(500);
                Ball_Servo_Reset();
                osDelay(500);
                Ball_Servo_Fire();
                osDelay(1000);
                Ball_Servo_Ready();
                osDelay(2000);
                // 状态转换
                handle_ball_state = Handle_Ball_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}