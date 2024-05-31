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

int pick_flag = 0; // 取球前馈标志

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
                pick_flag = 0;
                Ball_Servo_Grip();
                Ball_Servo_In();
                Ball_Servo_Ready();
                arm_angle           = -125;
                friction_speed_up   = 0;
                friction_speed_down = 0;
                while (((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) > 5.0f) ||
                       ((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) < -5.0f)) {
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
                osDelay(1000);
                Seed_Grip();
                Seed_Deposit_Close();
                Seed_Plant_Close();
                Seed_Deposit_Buffer_Open();
                Seed_Front_Close();
                motor_l_gripseed = -695;
                motor_r_gripseed = -695;
                while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 5.0f) ||
                       ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -5.0f) ||
                       ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 5.0f) ||
                       ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -5.0f)) {
                    osDelay(1);
                }
                motor_l_plantseed = 0;
                motor_r_plantseed = 0;
                while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 5.0f) ||
                       ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -5.0f) ||
                       ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 5.0f) ||
                       ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -5.0f)) {
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
                Ball_Servo_Reset();
                osDelay(5);
                // 使用下降PID
                // 速度环PID
                hDJI[4][0].speedPID.KP        = 10;
                hDJI[4][0].speedPID.KI        = 0.1;
                hDJI[4][0].speedPID.KD        = 5;
                hDJI[4][0].speedPID.outputMax = 8000;
                // 位置环PID
                hDJI[4][0].posPID.KP        = 1000.0f;
                hDJI[4][0].posPID.KI        = 10.00f;
                hDJI[4][0].posPID.KD        = 0.00f;
                hDJI[4][0].posPID.outputMax = 2000;
                arm_angle                   = -10;
                while (((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                       ((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                    osDelay(1);
                }
                pick_flag = 0;
                Ball_Servo_Out();
                osDelay(5);
                // 状态转换
                handle_ball_state = Handle_Ball_Fire;
                break;
            case Handle_Ball_Fire:
                // 消息发送
                JoystickSwitchTitle(ID_HANDLE_BALL, handle_ball_title, &mav_handle_ball_title);
                JoystickSwitchMsg(ID_HANDLE_BALL, handle_ball_fire_msg, &mav_handle_ball_msg);
                // 动作指令
                friction_speed_down = -1000;
                friction_speed_up   = 4000;
                osDelay(2000);
                Ball_Servo_Grip();
                osDelay(500);
                Ball_Servo_In();
                osDelay(400);
                // 使用上升PID
                // 速度环PID
                hDJI[4][0].speedPID.KP        = 12;
                hDJI[4][0].speedPID.KI        = 0.15;
                hDJI[4][0].speedPID.KD        = 5;
                hDJI[4][0].speedPID.outputMax = 8000;
                hDJI[4][0].speedPID.outputMin = -8000;
                // 位置环PID
                hDJI[4][0].posPID.KP        = 500.0f;
                hDJI[4][0].posPID.KI        = 10.00f;
                hDJI[4][0].posPID.KD        = 0.00f;
                hDJI[4][0].posPID.outputMax = 5000;
                arm_angle = -125;
                pick_flag = 1;
                if (hDJI[4][0].AxisData.AxisAngle_inDegree < -30.0f) {
                    pick_flag = 0;
                }
                while (((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                       ((hDJI[4][0].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                    osDelay(1);
                }
                Ball_Servo_Reset();
                osDelay(1000);
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