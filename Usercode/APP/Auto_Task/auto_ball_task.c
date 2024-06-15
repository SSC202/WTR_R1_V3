#include "auto_ball_task.h"

enum Auto_Ball_State auto_ball_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_ball_TaskHandle;
const osThreadAttr_t auto_ball_Task_attributes = {
    .name       = "auto_ball_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};
void m_auto_ball_Task(void *argument);

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
                while (btn_Btn0 != 1) {
                    osDelay(1);
                }
                chassis_x_pid.SetPoint = 250;
                chassis_y_pid.SetPoint = 0;
                while ((chassis_x_point - chassis_x_pid.SetPoint < -10.0f) || (chassis_x_point - chassis_x_pid.SetPoint > 10.0f)) {
                    osDelay(1);
                }
                // 状态转换
                auto_ball_state = Auto_Ball_Fire;
                break;
            case Auto_Ball_Fire:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_BALL, auto_ball_title, &mav_auto_ball_title);
                JoystickSwitchMsg(ID_AUTO_BALL, auto_ball_fire_msg, &mav_auto_ball_msg);
                osDelay(1);
                // TODO：加入动作
                while (btn_Btn1 != 1) {
                    osDelay(1);
                }
                chassis_x_pid.SetPoint = 152;
                chassis_y_pid.SetPoint = 1240;
                osDelay(1);
                auto_ball_state = Auto_Ball_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}