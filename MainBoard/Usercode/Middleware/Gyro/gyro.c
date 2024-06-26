#include "gyro.h"

osThreadId_t chassis_gyro_TaskHandle;
const osThreadAttr_t chassis_gyro_Task_attributes = {
    .name       = "chassis_gyro_Task",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

float chassis_offset       = 0; // 底盘初始角度
float chassis_yaw          = 0; // 底盘偏航角
uint8_t chassis_gyro_state = 0;

/**
 * @brief   陀螺仪初始化
 */
void m_Chassis_Gyro_Init(void)
{
    int cnt = 0;
    if (HW101_Init() != HAL_OK) {
        cnt++;
        if (cnt == 500) {
            HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
            cnt = 0;
        }
        osDelay(1);
    }
}

/**
 * @brief   陀螺仪线程开启
 */
void m_Chassis_Gyro_TaskStart(void)
{
    chassis_gyro_TaskHandle = osThreadNew(m_Chassis_Gyro_Task, NULL, &chassis_gyro_Task_attributes);
}

/**
 * @brief   陀螺仪线程
 */
void m_Chassis_Gyro_Task(void *argument)
{
    // 陀螺仪进行方向调节
    static float chassis_offset_sum = 0;
    chassis_gyro_state              = 0;
    osDelay(3000);
    for (;;) {
        if (chassis_gyro_state != 1) {
            // 获取偏航角偏移量
            for (int i = 0; i < 20; i++) {
                ProcessData();
                chassis_yaw = gyrodata[2];
                chassis_offset_sum += chassis_yaw;
                osDelay(10);
            }
            chassis_offset     = chassis_offset_sum / 20.0f;
            chassis_gyro_state = 1;
        } else {
            ProcessData();
            chassis_yaw = gyrodata[2];
        }
        osDelay(1);
    }
}