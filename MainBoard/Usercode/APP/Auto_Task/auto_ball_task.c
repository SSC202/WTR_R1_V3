/****************
 * 自动球点编号
 *      1 2 3 4 5 6
 *      7 8 9 10 11 12
 *  7 需要特殊处理
 */
#include "auto_ball_task.h"

enum Auto_Ball_State auto_ball_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_ball_TaskHandle;
const osThreadAttr_t auto_ball_Task_attributes = {
    .name       = "auto_ball_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_auto_ball_Task(void *argument);

uint8_t up_flag = 0; // 上坡运动状态标志位
uint8_t go_flag = 0; // 上坡状态标志位

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   自动取放球线程初始化
 */
void m_auto_ball_Task_Start(void)
{
    auto_ball_state      = Auto_Ball_Ready;
    auto_ball_TaskHandle = osThreadNew(m_auto_ball_Task, NULL, &auto_ball_Task_attributes);
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放球线程
 */
void m_auto_ball_Task(void *argument)
{
    static int ball_error_x;
    static int ball_error_y;
    for (;;) {
        switch (auto_ball_state) {
            case Auto_Ball_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_BALL, auto_ball_title, &mav_auto_ball_title);
                JoystickSwitchMsg(ID_AUTO_BALL, auto_ball_ready_msg, &mav_auto_ball_msg);
                osDelay(1);
                // 复位状态
                Reset_Action();
                // 状态机转换
                if (btn_KnobR == 1) {
                    auto_ball_state = Auto_Ball_Go;
                }
                break;
            case Auto_Ball_Go:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_BALL, auto_ball_title, &mav_auto_ball_title);
                JoystickSwitchMsg(ID_AUTO_BALL, auto_ball_go_msg, &mav_auto_ball_msg);
                osDelay(1);
                // 自动射球动作
                if (go_flag == 0) {
                    // 1. 到达坡前航路点
                    chassis_x_pid.SetPoint = 1450;
                    if (general_state == LEFT_MODE) {
                        chassis_y_pid.SetPoint = 600;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_y_pid.SetPoint = -600;
                    }
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -20.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 20.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 2. 上坡
                    up_flag = 1;
                    osDelay(2400);
                    up_flag = 2;
                    // 3. 校正
                    osDelay(1200);
                    OPS_Update_X(0);
                    osDelay(20);
                    OPS_Update_Y(0);
                    osDelay(1200);
                    ball_error_x = chassis_laser_x - 530;
                    ball_error_y = chassis_laser_y - 1390;
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = chassis_x_point + ball_error_x;
                        chassis_y_pid.SetPoint   = chassis_y_point + ball_error_y;
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = chassis_x_point + ball_error_x;
                        chassis_y_pid.SetPoint   = chassis_y_point - ball_error_y;
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    up_flag = 0;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 1.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -1.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    up_flag = 2;
                    OPS_Update_X(0);
                    osDelay(20);
                    OPS_Update_Y(0);
                    chassis_x_pid.SetPoint = 0;
                    chassis_y_pid.SetPoint = 0;
                    up_flag                = 0;
                    osDelay(100);
                    go_flag = 1;
                }
                while (btn_LeftCrossLeft == 0 &&
                       btn_LeftCrossMid == 0 &&
                       btn_LeftCrossUp == 0 &&
                       btn_LeftCrossDown == 0 &&
                       btn_LeftCrossRight == 0 &&
                       btn_RightCrossLeft == 0 &&
                       btn_RightCrossMid == 0 &&
                       btn_RightCrossUp == 0 &&
                       btn_RightCrossDown == 0 &&
                       btn_RightCrossRight == 0 &&
                       btn_Btn0 == 0 &&
                       btn_Btn5 == 0) {
                    osDelay(1);
                }
                if (btn_LeftCrossLeft == 1) // 1号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[0];
                        chassis_y_pid.SetPoint   = left_ball_point_y[0];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[0];
                        chassis_y_pid.SetPoint   = right_ball_point_y[0];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 40;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    chassis_x_pid.SetPoint += 40;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 10.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 10.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_LeftCrossUp == 1) // 2号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[1];
                        chassis_y_pid.SetPoint   = left_ball_point_y[1];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[1];
                        chassis_y_pid.SetPoint   = right_ball_point_y[1];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 40;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    chassis_x_pid.SetPoint += 20;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 15.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 15.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_LeftCrossMid == 1) // 3号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[2];
                        chassis_y_pid.SetPoint   = left_ball_point_y[2];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[2];
                        chassis_y_pid.SetPoint   = right_ball_point_y[2];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 40;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    chassis_x_pid.SetPoint += 40;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 18.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 18.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_LeftCrossDown == 1) // 4号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[3];
                        chassis_y_pid.SetPoint   = left_ball_point_y[3];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[3];
                        chassis_y_pid.SetPoint   = right_ball_point_y[3];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 40;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 40;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 25.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 25.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_LeftCrossRight == 1) // 5号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[4];
                        chassis_y_pid.SetPoint   = left_ball_point_y[4];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[4];
                        chassis_y_pid.SetPoint   = right_ball_point_y[4];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 40;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 40;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 45.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 45.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_Btn0 == 1) // 6号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[5];
                        chassis_y_pid.SetPoint   = left_ball_point_y[5];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[5];
                        chassis_y_pid.SetPoint   = right_ball_point_y[5];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 40;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1200;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 40;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 50.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 50.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_Btn5 == 1) // 7号点位
                {
                    chassis_x_pid.SetPoint = 0;
                    chassis_y_pid.SetPoint = 0;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[6];
                        chassis_y_pid.SetPoint   = left_ball_point_y[6];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[6];
                        chassis_y_pid.SetPoint   = right_ball_point_y[6];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 旋转
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 90.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 90.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    if (general_state == LEFT_MODE) {
                        chassis_y_pid.SetPoint -= 100;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_y_pid.SetPoint += 100;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    if (general_state == LEFT_MODE) {
                        chassis_y_pid.SetPoint += 100;
                        chassis_yaw_pid.SetPoint = chassis_offset + 5.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_y_pid.SetPoint -= 100;
                        chassis_yaw_pid.SetPoint = chassis_offset - 5.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_x_pid.SetPoint   = 0;
                    chassis_y_pid.SetPoint   = 0;
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_RightCrossLeft == 1) // 8号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[7];
                        chassis_y_pid.SetPoint   = left_ball_point_y[7];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[7];
                        chassis_y_pid.SetPoint   = right_ball_point_y[7];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 20;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1000;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 20;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 5.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 5.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_RightCrossUp == 1) // 9号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[8];
                        chassis_y_pid.SetPoint   = left_ball_point_y[8];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[8];
                        chassis_y_pid.SetPoint   = right_ball_point_y[8];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 20;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1200;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 20;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 12.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 12.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_RightCrossMid == 1) // 10号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[9];
                        chassis_y_pid.SetPoint   = left_ball_point_y[9];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[9];
                        chassis_y_pid.SetPoint   = right_ball_point_y[9];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 20;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1200;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 20;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 15.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 15.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_RightCrossDown == 1) // 11号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[10];
                        chassis_y_pid.SetPoint   = left_ball_point_y[10];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[10];
                        chassis_y_pid.SetPoint   = right_ball_point_y[10];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 20;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1200;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 20;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 30.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 30.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                } else if (btn_RightCrossRight == 1) // 12号点位
                {
                    if (general_state == LEFT_MODE) {
                        chassis_x_pid.SetPoint   = left_ball_point_x[11];
                        chassis_y_pid.SetPoint   = left_ball_point_y[11];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_x_pid.SetPoint   = right_ball_point_x[11];
                        chassis_y_pid.SetPoint   = right_ball_point_y[11];
                        chassis_yaw_pid.SetPoint = chassis_offset;
                    }
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -5.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -5.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 5.0f)) {
                        osDelay(1);
                    }
                    // 倒退
                    chassis_x_pid.SetPoint -= 20;
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    // 取球
                    Ball_Pick_Action();
                    // 射球
                    friction_speed_down = -1200;
                    friction_speed_up   = 4000;
                    osDelay(2000);
                    Ball_Servo_Grip();
                    osDelay(500);
                    arm_angle = -140;
                    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
                           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_In();
                    osDelay(1000);
                    Ball_Servo_Reset();
                    // 前进并转向
                    chassis_x_pid.SetPoint += 20;
                    if (general_state == LEFT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset + 50.0f;
                    } else if (general_state == RIGHT_MODE) {
                        chassis_yaw_pid.SetPoint = chassis_offset - 50.0f;
                    }
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint < -3.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -3.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 3.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Fire();
                    osDelay(1000);
                    chassis_yaw_pid.SetPoint = chassis_offset;
                    while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                           (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                        osDelay(1);
                    }
                    Ball_Servo_Ready();
                }
                // 状态转换
                auto_ball_state = Auto_Ball_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}