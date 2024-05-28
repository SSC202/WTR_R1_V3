//
// Created by liam on 2023/10/3.
//
/**
 * @file Unitree_user.c
 * @author Liam (quan.2003@outlook.com)
 * @brief Unitree_M8010-6电机用户端代码
 * @version 1.0
 * @data 2023-10-04
 *
 * @copyright Copyright (c) 2023
 *
 * Unitree Go电机使用说明：
 * 定义串口       （通信速率4Mbit/s，须在CubeMX中设置UART的时钟为 160MHz，如果使用其他时钟频率请确保波特率生成的误差小于 2%）
 * 开启DMA_Rx     (提高传输成功率)
 * 配置 RE DE引脚，并设置GPIO_OUTPUT_SPEED  Very High
 * 该电机初始位置不确定需要进行  零点调整
 *
 * 该电机使用 力位混合模式
 * 控制电机公式为：
 *     f = f_ + k_p * (p_des - p) + k_d * (w_des - w)
 *
 * 代码说明（头文件Unitree_user.h）：
 * 驱动电机只需要在Unitree_User.h  头文件中定义：
 * 通信使用的串口
 * RE 引脚        接收使能（低电平有效）
 * DE 引脚        发送使能（高电平有效）
 *
 * 注意：
 * 接线请使用可靠插件，保证信号线 接地信号纯净（杜邦线在高速串口传输下并不可靠！）
 * Rx485线 需使用 差分数据线
 * 请注意 Rx485_TTL模块 是否支持4Mbit/s 波特率
 */
#define USE_FREERTOS

#include "Unitree_user.h"
#ifdef USE_FREERTOS
#include "cmsis_os.h"
#else
#include "stdlib.h"
#endif
/**************************************变量定义**********************************************/
__section("._D1_Area") UnitreeMotor unitree_motor_right;
__section("._D1_Area") UnitreeMotor unitree_motor_left;
float unitree_offset_right;
float unitree_offset_left;
/**************************************用户函数**********************************************/
/**
 * @brief   创建电机对象
 */
// UnitreeMotor *Unitree_Create_Motor()
// {
// #ifdef USE_FREERTOS
//     UnitreeMotor *instance = (UnitreeMotor *)pvPortMalloc(sizeof(UnitreeMotor));
// #else
//     UnitreeMotor *instance = (UnitreeMotor *)malloc(sizeof(UnitreeMotor));
// #endif
//     return instance;
// }

/**
 * 电机初始化自检
 * @return
 */
HAL_StatusTypeDef Unitree_init(UnitreeMotor *MotorInstance, UART_HandleTypeDef *usartx, int id)
{
    MotorInstance->cmd.id   = id; // 给电机控制指令结构体赋值
    MotorInstance->cmd.mode = 1;
    MotorInstance->cmd.T    = 0;
    MotorInstance->cmd.W    = 0;
    MotorInstance->cmd.Pos  = 0;
    MotorInstance->cmd.K_P  = 0;
    MotorInstance->cmd.K_W  = 0.05;
    MotorInstance->huart    = usartx;
    if (SERVO_Send_recv(MotorInstance->huart, &MotorInstance->cmd, &MotorInstance->data) == 0 && MotorInstance->data.MError == 0) {
        MotorInstance->offset = MotorInstance->data.Pos;
        return HAL_OK;
    } else
        return HAL_ERROR;
}

/**
 * 电机自定义参数发送函数
 * @param MotorInstance  电机对象实例
 * @param motor_id       电机ID （0-14）
 * @param mode           电机工作模式 （0.锁定；1.FOC闭环；2.编码器校准）
 * @param T              电机转矩（-127.99 ~ 127.99  单位 N·m）
 * @param W              电机输出轴速度（-127.014 ~ 127.014  单位 rad/s）
 * @param Pos            电机输出轴输出位置（-65051.18 ~ 65051.18  单位 rad）
 * @param K_P            电机刚度系数/位置误差比例系数（0 ~ 25.599）
 * @param K_W            电机阻尼系数/速度误差比例（0 ~ 25.599）
 * @return               函数状态
 */
HAL_StatusTypeDef Unitree_UART_tranANDrev(UnitreeMotor *MotorInstance, unsigned char motor_id,
                                          unsigned char mode, float T, float W,
                                          float Pos, float K_P, float K_W)
{
    MotorInstance->cmd.id   = motor_id;
    MotorInstance->cmd.mode = mode;
    MotorInstance->cmd.T    = T;
    MotorInstance->cmd.W    = W * UNITREE_REDUCTION_RATE;
    MotorInstance->cmd.Pos  = Pos * UNITREE_REDUCTION_RATE;
    MotorInstance->cmd.K_P  = K_P;
    MotorInstance->cmd.K_W  = K_W;
    HAL_StatusTypeDef ret;
    ret = SERVO_Send_recv(MotorInstance->huart, &(MotorInstance->cmd), &(MotorInstance->data));
    if (ret == HAL_OK)
        MotorInstance->data.Pos -= MotorInstance->offset;
    return ret;
}

/**
 * 编码器校正
 * @param MotorInstance
 * @return
 */
HAL_StatusTypeDef Unitree_Encoder_Autoclibrating(UnitreeMotor *MotorInstance)
{
    HAL_StatusTypeDef state_ = HAL_OK;
    state_                   = Unitree_UART_tranANDrev(MotorInstance, MotorInstance->cmd.id, 2, 0, 0, 0, 0, 0);
#ifdef USE_FREERTOS
    osDelay(6000);
#else
    HAL_Delay(6000);
#endif
    return state_;
}

/***********************************************************************************************/

/**
 * @brief   Unitree 初始化位置确定
 */
void Unitree_User_Init(void)
{
    int cnt = 0;
    osDelay(2);
    // unitree_motor_left = Unitree_Create_Motor();
    osDelay(2);
    while (Unitree_init(&unitree_motor_right, &UART_UNITREE_HANDLER, 0) != HAL_OK) {
        cnt++;
        if (cnt == 500) {
            HAL_GPIO_TogglePin(LED6_GPIO_Port, LED6_Pin);
            cnt = 0;
        }
        osDelay(1);
    }
    // while (Unitree_init(unitree_motor_left, &UART_UNITREE_HANDLER, 1) != HAL_OK) {
    //     cnt++;
    //     if (cnt == 500) {
    //         HAL_GPIO_TogglePin(LED6_GPIO_Port, LED6_Pin);
    //         cnt = 0;
    //     }
    //     osDelay(1);
    // }
    // Unitree_Encoder_Autoclibrating(&unitree_motor_right);
    // Unitree_Encoder_Autoclibrating(unitree_motor_left);
    // osDelay(7000);
    Unitree_UART_tranANDrev(&unitree_motor_right, 0, 0, 0, 0, 0, 0, 0);
    // osDelay(2);
    // Unitree_UART_tranANDrev(unitree_motor_left, 1, 0, 0, 0, 0, 0, 0);
    osDelay(2);
    unitree_offset_right = unitree_motor_right.data.Pos;
    // unitree_offset_left     = unitree_motor_left->data.Pos;
    // unitree_motor_left_pos  = unitree_offset_left;
    // unitree_motor_right_pos = unitree_offset_right;
}

// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     if (huart->Instance == &UART_UNITREE_HANDLER) {
//         uart_decode();
//         SCB_InvalidateDCache();
//         //__HAL_UNLOCK(huart);
//         HAL_UARTEx_ReceiveToIdle_DMA(&UART_UNITREE_HANDLER, (uint8_t *)buffer, 512);
//     }
// }