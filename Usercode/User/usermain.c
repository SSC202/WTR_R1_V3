/********************************************************************
 * @brief   WTR2024 R1 机械臂版本工程
 * @note
 */
#include "usermain.h"

void StartDefaultTask(void *argument)
{
    // Hardware Init
    m_RemoteCtl_Init(); // 遥控器初始化
    m_Chassis_Init();   // DJI电机初始化
    osDelay(1000);
    m_Unitree_Init();   // Unitree电机初始化

    // Task start
    m_RemoteCtl_Task_Start();          // 遥控器线程
    m_Chassis_CAN_Message_TaskStart(); // CAN通信线程
    osDelay(2000);
    m_Unitree_Ctl_Message_TaskStart();
    m_Unitree_UART_Message_TaskStart();


    // Left and Right choose
    do {
        // 发送等待消息
        JoystickSwitchTitle(ID_DIRECT_CHOOSE, direct_choose_title, &mav_dir_choose_title);
        JoystickSwitchMsg(ID_DIRECT_CHOOSE, direct_choose_msg, &mav_dir_choose_msg);
        // 等待选择
        if (btn_KnobR == 1 && usr_right_x > 500.0f) {
            // TODO: Download Right Mode Data

            general_state = RIGHT_MODE;                          // 指定为右侧状态
            JoystickDelete(ID_DIRECT_CHOOSE, &mav_joystick_del); // 取消等待指令
            break;
        } else if (btn_KnobR == 1 && usr_right_x < -500.0f) {
            // TODO: Download Left Mode Data

            general_state = LEFT_MODE;                           // 指定为左侧状态
            JoystickDelete(ID_DIRECT_CHOOSE, &mav_joystick_del); // 取消等待指令
            break;
        }
        osDelay(1);
    } while (1);

    // TODO: calibration

    // main task entry point

    m_main_Task_Start();
    // main run
    static int i = 0;
    for (;;) {
        // Run State
        i++;
        if (i == 1000) {
            i = 0;
            // printf("%d\r\n", usr_left_y);
            HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
        }
        osDelay(1);
    }
}
