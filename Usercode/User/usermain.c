/********************************************************************
 * @brief   WTR2024 R1 机械臂版本工程
 * @note
 */
#include "usermain.h"

char debug_title[20] = "Debug";
char debug_msg[20];
mavlink_joystick_air_dashboard_set_title_t mav_debug_title;
mavlink_joystick_air_dashboard_set_msg_t mav_debug_msg;

void StartDefaultTask(void *argument)
{
    // Hardware Init
    m_RemoteCtl_Init(); // 遥控器初始化
    m_Chassis_Init();   // DJI电机初始化
    m_Servo_Init();     // 舵机初始化
    osDelay(1000);
    m_Unitree_Init();      // Unitree电机初始化
    m_Chassis_Gyro_Init(); // 陀螺仪初始化
    osDelay(7000);
    m_Chassis_Odom_Init(); // 码盘初始化

    // Task start
    osDelay(7000);              // 上电后等待系统稳定
    m_RemoteCtl_Task_Start();   // 遥控器线程
    m_Chassis_Gyro_TaskStart(); // 陀螺仪线程
    while (chassis_gyro_state != 1) {
        osDelay(1);
    }
    m_Chassis_CAN_Message_TaskStart();  // CAN通信线程
    m_Chassis_Ctl_TaskStart();          // 底盘控制线程
    m_Unitree_Ctl_Message_TaskStart();  // Unitree电机控制线程
    m_Unitree_UART_Message_TaskStart(); // Unitree电机通信线程
    m_Chassis_Odom_TaskStart();         // 码盘坐标转换线程

    // Left and Right choose
    do {
        // 发送等待消息
        JoystickSwitchTitle(ID_DIRECT_CHOOSE, direct_choose_title, &mav_dir_choose_title);
        JoystickSwitchMsg(ID_DIRECT_CHOOSE, direct_choose_msg, &mav_dir_choose_msg);
        // 等待选择
        if (btn_KnobR == 1 && usr_right_x > 500.0f) {
            general_state = RIGHT_MODE;                          // 指定为右侧状态
            JoystickDelete(ID_DIRECT_CHOOSE, &mav_joystick_del); // 取消等待指令
            break;
        } else if (btn_KnobR == 1 && usr_right_x < -500.0f) {
            general_state = LEFT_MODE;                           // 指定为左侧状态
            JoystickDelete(ID_DIRECT_CHOOSE, &mav_joystick_del); // 取消等待指令
            break;
        }
        osDelay(1);
    } while (1);

    // main task entry point
    m_main_Task_Start();
    // main run
    static int i = 0;
    for (;;) {
        // Run State
        i++;
        if (i == 1000) {
            i = 0;
            HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
        }
        sprintf(debug_msg, "y:%d", (int)(usr_left_y));
        JoystickSwitchTitle(10, debug_title, &mav_debug_title);
        JoystickSwitchMsg(10, debug_msg, &mav_dir_choose_msg);
        osDelay(1);
    }
}
