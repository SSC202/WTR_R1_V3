/********************************************************************
 * @brief   WTR2024 R1 机械臂版本工程
 * @note
 */
#include "usermain.h"

void StartDefaultTask(void *argument)
{
    // Hardware Init
    

    static int i = 0;
    for (;;) {
        // Run State
        i++;
        if (i == 1000) {
            i = 0;
            // printf("%d\r\n", usr_left_y);
            HAL_GPIO_TogglePin(LED7_GPIO_Port, LED7_Pin);
        }
        osDelay(1);
    }
}
