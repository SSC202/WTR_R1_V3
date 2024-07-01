#include "action.h"

/************************************
 * @brief   总复位状态
 */
void Reset_Action(void)
{
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
    arm_angle           = -125;
    friction_speed_up   = 0;
    friction_speed_down = 0;
    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 5.0f) ||
           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -5.0f)) {
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
    motor_l_gripseed = -995;
    motor_r_gripseed = -995;
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
}

/************************************
 * @brief   取苗动作
 */
void Seed_Grip_Action(void)
{
    // 取苗动作
    /******************************************
     * 准备取苗动作
     *      1. 宇树电机取苗准备位置
     *      2. 夹爪放松
     *      3. 取苗门打开
     *      4. 电机降低
     */
    if (seed_count < 6) {
        unitree_right_pos = PI / 2 + 0.3;
        unitree_left_pos  = -PI / 2 - 0.3;
        osDelay(2000);
        Seed_Deposit();
        osDelay(1000);
        motor_l_gripseed = -5;
        motor_r_gripseed = -5;
        while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
               ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
            osDelay(1);
        }
    }
    /*******************************************
     * 取苗动作
     */
    // 第一段动作
    if (seed_count < 6) {
        osDelay(2000);
        unitree_right_pos = PI / 2 - 0.2;
        unitree_left_pos  = -PI / 2 + 0.2;
        osDelay(500);
        Seed_Grip();
        osDelay(500);
        Seed_Deposit_Open();
        osDelay(500);
        motor_l_gripseed = -995;
        motor_r_gripseed = -995;
        while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
               ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
               ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
            osDelay(1);
        }
    }
    // 第二段动作
    if (seed_count < 4) {
        osDelay(2000);
        unitree_right_pos = -PI / 2 - 0.2;
        unitree_left_pos  = PI / 2 + 0.2;
        osDelay(2000);
        Seed_Deposit();
        osDelay(500);
        unitree_right_pos = PI / 4;
        unitree_left_pos  = -PI / 4;
        Seed_Deposit_Close();
        osDelay(200);
        Seed_Deposit_Buffer_Close();
        osDelay(5000);
        motor_l_plantseed = 385;
        motor_r_plantseed = -385;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        osDelay(300);
        motor_l_plantseed = 2;
        motor_r_plantseed = -2;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        Seed_Deposit_Buffer_Open();
        osDelay(1000);
        seed_count++;
    } else if (seed_count == 4) {
        osDelay(2000);
        unitree_right_pos = -PI / 2 - 0.2;
        unitree_left_pos  = PI / 2 + 0.2;
        osDelay(1000);
        Seed_Deposit();
        unitree_right_pos = PI / 4;
        unitree_left_pos  = -PI / 4;
        Seed_Deposit_Close();
        seed_count++;
    } else if (seed_count == 5) {
        osDelay(2000);
        unitree_right_pos = PI / 4;
        unitree_left_pos  = -PI / 4;
        Seed_Grip();
        seed_count = 6;
    } else if (seed_count == 6) {
        ;
    }
}

/************************************
 * @brief   取苗动作
 */
void Seed_Plant_Action(void)
{
    /***********************
     * 放苗动作
     *
     */
    // 第一段
    osDelay(2000);
    Seed_Front_Open();
    Seed_Plant_Open();
    osDelay(2000);
    Seed_Plant_Close();
    osDelay(2000);
    Seed_Front_Close();
    // 移动至下一个苗进入放苗口
    if (seed_count == 6) {
        Seed_Deposit_Buffer_Close();
        motor_l_plantseed = 385;
        motor_r_plantseed = -385;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        osDelay(500);
        motor_l_plantseed = 2;
        motor_r_plantseed = -2;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        Seed_Deposit_Open();
        osDelay(5);
        Seed_Deposit_Buffer_Open();
        osDelay(5);
        unitree_right_pos = -PI / 2 - 0.2;
        unitree_left_pos  = PI / 2 + 0.2;
        osDelay(1000);
        Seed_Deposit();
        osDelay(5);
        Seed_Deposit_Close();
        osDelay(5);
        unitree_right_pos = PI / 4;
        unitree_left_pos  = -PI / 4;
        osDelay(2000);
        Seed_Deposit_Close();
        osDelay(5);
        seed_count--;
    } else if (seed_count == 5) {
        Seed_Deposit_Buffer_Close();
        osDelay(5);
        motor_l_plantseed = 385;
        motor_r_plantseed = -385;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        motor_l_plantseed = 2;
        motor_r_plantseed = -2;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        Seed_Deposit_Buffer_Open();
        osDelay(5);
        seed_count--;
    } else if (seed_count < 5 && seed_count > 0) {
        Seed_Deposit_Buffer_Close();
        osDelay(5);
        motor_l_plantseed = deposit_point[seed_count - 1];
        motor_r_plantseed = -deposit_point[seed_count - 1];
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        osDelay(500);
        motor_l_plantseed = 2;
        motor_r_plantseed = -2;
        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
            osDelay(1);
        }
        Seed_Deposit_Buffer_Open();
        osDelay(5);
        seed_count--;
    } else if (seed_count == 0) {
        ;
    }
    osDelay(2000);
}

/**
 * @brief   取球动作
 */
void Ball_Pick_Action(void)
{
    Ball_Servo_Reset();
    osDelay(5);
    arm_angle = -10;
    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
        osDelay(1);
    }
    Ball_Servo_Out();
    osDelay(5);
}

/**
 * @brief   射球动作
 */
void Ball_Fire_Action(void)
{
    friction_speed_down = -800;
    friction_speed_up   = 5000;
    osDelay(2000);
    Ball_Servo_Grip();
    osDelay(500);
    arm_angle = -125;
    while (((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) > 2.0f) ||
           ((hDJI[4][1].AxisData.AxisAngle_inDegree - arm_angle) < -2.0f)) {
        osDelay(1);
    }
    osDelay(500);
    Ball_Servo_In();
    osDelay(1000);
    Ball_Servo_Reset();
    osDelay(1000);
    Ball_Servo_Fire();
    osDelay(1000);
    Ball_Servo_Ready();
    osDelay(500);
}