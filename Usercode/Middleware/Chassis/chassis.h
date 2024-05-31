#ifndef __CHASSIS_H
#define __CHASSIS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "usermain.h"
#include "Caculate.h"
#include "math.h"
#include "wtr_can.h"

/****************************************************************************
 * 接口定义
 */

extern int arm_angle;            // 机械臂角度
extern int friction_speed_up;    // 上摩擦轮速度
extern int friction_speed_down;  // 下摩擦轮速度
extern float v_1, v_2, v_3, v_4; // 底盘速度
extern float motor_r_gripseed;   // 右侧取苗电机位置
extern float motor_l_gripseed;   // 左侧取苗电机位置
extern float motor_r_plantseed;  // 右侧放苗电机位置
extern float motor_l_plantseed;  // 左侧放苗电机位置

/*************************************************************
 * 用户函数
 */

void m_Chassis_CAN_Message_TaskStart(void);
void m_Chassis_Ctl_TaskStart(void);
void m_Chassis_Init(void);

void Inverse_kinematic_equation(float vx, float vy, float wc, float *_v_1, float *_v_2, float *_v_3, float *_v_4);

#ifdef __cplusplus
}
#endif
#endif