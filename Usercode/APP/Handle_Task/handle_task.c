#include "handle_task.h"

enum Handle_State handle_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t handle_TaskHandle;
const osThreadAttr_t handle_Task_attributes = {
    .name       = "handle_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_handle_Task(void *argument);

/*****************************************************************************
 * 接口函数定义
 */
/**
 * @brief   总线程初始化
 */
void m_handle_Task_Start(void)
{
    handle_state      = HANDLE_IDLE_MODE; // 初始化为空闲状态
    handle_TaskHandle = osThreadNew(m_handle_Task, NULL, &handle_Task_attributes);
}

/*****************************************************************************
 * 线程函数定义
 */
/**
 * @brief   手动线程
 */
void m_handle_Task(void *argument)
{
    /**
     * 左侧开关Switch指示自动/手动状态，在手动线程中，仅有空闲状态方能切换到自动状态
     * 右侧摇杆纵向值指示苗/球状态
     */
    m_handle_ball_Task_Start();
    m_handle_seed_Task_Start();
    for (;;) {
        // 手动线程管理
        switch (handle_state) {
            case HANDLE_IDLE_MODE: // 空闲状态
                // 挂起线程
                osThreadSuspend(handle_seed_TaskHandle);
                osThreadSuspend(handle_ball_TaskHandle);
                // 发送状态指令
                JoystickDelete(ID_HANDLE_SEED, &mav_joystick_del);
                JoystickDelete(ID_HANDLE_BALL, &mav_joystick_del);
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_idle_msg, &mav_mode_msg);
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
                // 等待转换指令
                // 1. 转换到自动状态
                if (left_switch == 1) {
                    auto_state = AUTO_IDLE_MODE;
                    run_state  = AUTO_MODE;
                }
                // 2. 转换到手动球状态
                if (usr_right_y > 500.0f) {
                    osThreadSuspend(handle_seed_TaskHandle);
                    osThreadResume(handle_ball_TaskHandle);
                    handle_state = HANDLE_BALL_MODE;
                }
                // 3. 转换到手动苗状态
                if (usr_right_y < -500.0f) {
                    osThreadSuspend(handle_ball_TaskHandle);
                    osThreadResume(handle_seed_TaskHandle);
                    handle_state = HANDLE_SEED_MODE;
                }
                break;
            case HANDLE_SEED_MODE:
                // 发送状态指令
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_seed_msg, &mav_mode_msg);
                if (btn_Btn4 == 1 && handle_seed_state == Handle_Seed_Ready) {
                    handle_state = HANDLE_IDLE_MODE;
                }
                break;
            case HANDLE_BALL_MODE:
                // 发送状态指令
                JoystickSwitchTitle(ID_MODE, mode_title, &mav_mode_title);
                JoystickSwitchMsg(ID_MODE, mode_ball_msg, &mav_mode_msg);
                // 1. 转换到空闲状态
                if (btn_Btn4 == 1 && handle_ball_state == Handle_Ball_Ready) {
                    handle_state = HANDLE_IDLE_MODE;
                }
                break;
            default:
                break;
        }
        static int i = 0;
        i++;
        if (i == 1000) {
            i = 0;
            HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
        }
        osDelay(1);
    }
}