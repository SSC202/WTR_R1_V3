#include "auto_seed_task.h"

enum Auto_Seed_State auto_seed_state;

enum Auto_Seed_Point_State auto_seed_point_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_seed_TaskHandle;
const osThreadAttr_t auto_seed_Task_attributes = {
    .name       = "auto_seed_Task",
    .stack_size = 256 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_auto_seed_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   手动取放苗线程初始化
 */
void m_auto_seed_Task_Start(void)
{
    auto_seed_state      = Auto_Seed_Ready;
    auto_seed_TaskHandle = osThreadNew(m_auto_seed_Task, NULL, &auto_seed_Task_attributes);
}

/*****************************************************************************
 * 线程函数
 */
/**
 * @brief   手动取放苗线程
 */
void m_auto_seed_Task(void *argument)
{
    for (;;) {
        switch (auto_seed_state) {
            case Auto_Seed_Ready:
                // 复位状态
                Reset_Action();
                // 状态转换
                if (btn_KnobR == 1 && usr_right_x < -500.0f) {
                    auto_seed_state = Auto_Seed_Grip;
                }
                if (btn_KnobR == 1 && usr_right_x > 500.0f) {
                    auto_seed_state = Auto_Seed_Plant;
                }
                break;
            case Auto_Seed_Grip:
                // 第一段：寻找第一航路点
                // 如果此时车点在苗架线以内，先运行到苗架线以外，反之则不用运行至第一航路点(250,250)
                if (chassis_x_point < 250.0f) {
                    auto_seed_point_state = Auto_Seed_Find_1Point;
                    osDelay(50);
                    // 运行至第一航路点
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -30.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 30.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -30.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 30.0f)) {
                        osDelay(1);
                    }
                }
                // 运行至第一取苗点
                auto_seed_point_state = Auto_Seed_Grip_Seed0;
                // 第二段：连续取苗
                for (int i = 0; i < 6; i++) {
                    // 1. 运行至航路点
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -20.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 20.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                        osDelay(1);
                    }
                    // 2. 开始取苗
                    /******************************************
                     * 准备取苗动作
                     *      1. 宇树电机取苗准备位置
                     *      2. 夹爪放松
                     *      3. 取苗门打开
                     *      4. 电机降低
                     */
                    unitree_right_pos = PI / 2 + 0.3;
                    unitree_left_pos  = -PI / 2 - 0.3;
                    Seed_Deposit();
                    osDelay(500);
                    motor_l_gripseed = -5;
                    motor_r_gripseed = -5;
                    while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
                           ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
                        osDelay(1);
                    }
                    /*******************************************
                     * 取苗动作
                     */
                    // 第一段动作
                    // 宇树电机运动至超过苗轴线
                    unitree_right_pos = PI / 2 - 0.2;
                    unitree_left_pos  = -PI / 2 + 0.2;
                    // 夹紧，开轨道门
                    osDelay(500);
                    Seed_Grip();
                    Seed_Deposit_Open();
                    osDelay(500);
                    // 将苗夹起
                    motor_l_gripseed = -695;
                    motor_r_gripseed = -695;
                    while (((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) > 2.0f) ||
                           ((hDJI[0][1].AxisData.AxisAngle_inDegree - motor_r_gripseed) < -2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) > 2.0f) ||
                           ((hDJI[1][1].AxisData.AxisAngle_inDegree - motor_l_gripseed) < -2.0f)) {
                        osDelay(1);
                    }
                    // 第二段动作
                    // 前4个苗
                    if (i == 0 || i == 1 || i == 2 || i == 3) {
                        unitree_right_pos = -PI / 2 - 0.2;
                        unitree_left_pos  = PI / 2 + 0.2;
                        osDelay(500);
                        // 此时允许运动至下一点
                        if (i == 0) {
                            auto_seed_point_state = Auto_Seed_Grip_Seed1;
                        } else if (i == 1) {
                            auto_seed_point_state = Auto_Seed_Grip_Seed2;
                        } else if (i == 2) {
                            auto_seed_point_state = Auto_Seed_Grip_Seed3;
                        } else if (i == 3) {
                            auto_seed_point_state = Auto_Seed_Grip_Seed4;
                        }
                        // 继续
                        osDelay(500);
                        Seed_Deposit();
                        osDelay(500);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        Seed_Deposit_Close();
                        osDelay(200);
                        Seed_Deposit_Buffer_Close();
                        osDelay(500);
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
                    }
                    // 第五个苗，不推
                    else if (i == 4) {
                        unitree_right_pos = -PI / 2 - 0.2;
                        unitree_left_pos  = PI / 2 + 0.2;
                        osDelay(500);
                        auto_seed_point_state = Auto_Seed_Grip_Seed5;
                        osDelay(500);
                        Seed_Deposit();
                        osDelay(500);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        osDelay(200);
                        Seed_Deposit_Close();
                        Seed_Deposit_Buffer_Close();
                        seed_count++;
                    }
                    // 最后一个苗，不进门
                    else if (i == 5) {
                        // 前往第二航路点
                        auto_seed_point_state = Auto_Seed_Grip_2Point;
                        unitree_right_pos     = PI / 4;
                        unitree_left_pos      = -PI / 4;
                        Seed_Grip();
                        seed_count = 6;
                    }
                    osDelay(1);
                }
                // 3. 等待到达第二航路点
                while ((chassis_x_point - chassis_x_pid.SetPoint < -30.0f) ||
                       (chassis_x_point - chassis_x_pid.SetPoint > 30.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint < -30.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint > 30.0f)) {
                    osDelay(1);
                }
                // 状态转换
                auto_seed_state = Auto_Seed_Plant;
                break;
            case Auto_Seed_Plant:
                // 2. 旋转
                auto_seed_point_state = Auto_Seed_Plant_2Point;
                while ((chassis_yaw - chassis_yaw_pid.SetPoint < -1.0f) ||
                       (chassis_yaw - chassis_yaw_pid.SetPoint > 1.0f)) {
                    osDelay(1);
                }
                osDelay(500);
                auto_seed_point_state = Auto_Seed_Plant_Seed0;
                // 3. 开始放苗
                for (int i = 0; i < 6; i++) {
                    // 等待到达航路点
                    while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                           (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                           (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                        osDelay(1);
                    }
                    // 放苗
                    /***********************
                     * 放苗动作
                     *
                     */
                    // 第一段
                    Seed_Deposit_Buffer_Close();
                    Seed_Front_Open();
                    Seed_Plant_Open();
                    osDelay(500);
                    // 前往下一航路点
                    if (i == 0) {
                        auto_seed_point_state = Auto_Seed_Plant_Seed1;
                    } else if (i == 1) {
                        auto_seed_point_state = Auto_Seed_Plant_Seed2;
                    } else if (i == 2) {
                        auto_seed_point_state = Auto_Seed_Plant_Seed3;
                    } else if (i == 3) {
                        auto_seed_point_state = Auto_Seed_Plant_Seed4;
                    } else if (i == 4) {
                        auto_seed_point_state = Auto_Seed_Plant_Seed5;
                    } else if (i == 5) {
                        auto_seed_point_state = Auto_Seed_Plant_3Point;
                    }
                    // 脱离接触后关闭放苗门
                    osDelay(1500);
                    Seed_Plant_Close();
                    Seed_Front_Close();
                    osDelay(500);
                    if (i == 0) {
                        // 第一次，先推苗
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
                        Seed_Deposit_Open();
                        osDelay(500);
                        unitree_right_pos = -PI / 2 - 0.2;
                        unitree_left_pos  = PI / 2 + 0.2;
                        osDelay(500);
                        Seed_Deposit();
                        osDelay(500);
                        unitree_right_pos = PI / 4;
                        unitree_left_pos  = -PI / 4;
                        Seed_Grip();
                        Seed_Deposit_Close();
                        osDelay(200);
                        Seed_Deposit_Buffer_Close();
                    } else if (i == 1) {
                        Seed_Deposit_Buffer_Close();
                        motor_l_plantseed = 385;
                        motor_r_plantseed = -385;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        osDelay(300);
                    } else if (i == 2) {
                        Seed_Deposit_Buffer_Close();
                        motor_l_plantseed = 590;
                        motor_r_plantseed = -590;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        osDelay(300);
                    } else if (i == 3) {
                        Seed_Deposit_Buffer_Close();
                        motor_l_plantseed = 770;
                        motor_r_plantseed = -770;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        osDelay(300);
                    } else if (i == 4) {
                        Seed_Deposit_Buffer_Close();
                        motor_l_plantseed = 960;
                        motor_r_plantseed = -960;
                        while (((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) > 2.0f) ||
                               ((hDJI[2][1].AxisData.AxisAngle_inDegree - motor_r_plantseed) < -2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) > 2.0f) ||
                               ((hDJI[3][1].AxisData.AxisAngle_inDegree - motor_l_plantseed) < -2.0f)) {
                            osDelay(1);
                        }
                        osDelay(300);
                    } else if (i == 5) {
                        Seed_Deposit_Buffer_Close();
                    }
                }
                // 等待到达航路点
                while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) ||
                       (chassis_x_point - chassis_x_pid.SetPoint > 10.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint < -10.0f) ||
                       (chassis_y_point - chassis_y_pid.SetPoint > 10.0f)) {
                    osDelay(1);
                }
                auto_seed_point_state = Auto_Seed_Plant_4Point;
                while ((chassis_yaw - chassis_yaw_pid.SetPoint < -3.0f) ||
                       (chassis_yaw - chassis_yaw_pid.SetPoint > 3.0f)) {
                    osDelay(1);
                }
                // 状态转换
                auto_seed_state = Auto_Seed_Ready;
                break;
            default:
                break;
        }
        switch (auto_seed_state) {
            case Auto_Seed_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_ready_msg, &mav_auto_seed_msg);
                break;
            case Auto_Seed_Grip:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_grip_msg, &mav_auto_seed_msg);
                break;
            case Auto_Seed_Plant:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_plant_msg, &mav_auto_seed_msg);
                break;
            default:
                break;
        }
        osDelay(1);
    }
}