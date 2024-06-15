#include "auto_seed_task.h"

enum Auto_Seed_State auto_seed_state;
/*****************************************************************************
 *  线程定义
 */
osThreadId_t auto_seed_TaskHandle;
const osThreadAttr_t auto_seed_Task_attributes = {
    .name       = "auto_seed_Task",
    .stack_size = 128 * 4,
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
            case Handle_Seed_Ready:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_ready_msg, &mav_auto_seed_msg);
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
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_grip_msg, &mav_auto_seed_msg);
                // TODO：取苗动作
                for (int i = 0; i < 4; i++) {
                    HAL_GPIO_TogglePin(LED6_GPIO_Port, LED6_Pin);
                    osDelay(500);
                }
                // 状态转换
                auto_seed_state = Auto_Seed_Ready;
                break;
            case Auto_Seed_Plant:
                // 消息发送
                JoystickSwitchTitle(ID_AUTO_SEED, auto_seed_title, &mav_auto_seed_title);
                JoystickSwitchMsg(ID_AUTO_SEED, auto_seed_plant_msg, &mav_auto_seed_msg);
                // TODO：放苗动作
                for (int i = 0; i < 4; i++) {
                    HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
                    osDelay(500);
                }
                // 状态转换
                auto_seed_state = Auto_Seed_Ready;
                break;
            default:
                break;
        }
        osDelay(1);
    }
}