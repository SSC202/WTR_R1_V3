#ifndef __AUTO_SEED_TASK_H
#define __AUTO_SEED_TASK_H
#ifdef __cplusplus
extern "C" {
#endif
// C

#include "usermain.h"

enum Auto_Seed_State {
    Auto_Seed_Ready,
    Auto_Seed_Go,
};
extern enum Auto_Seed_State auto_seed_state;

enum Auto_Seed_Point_State {
    Auto_Seed_Find_1Point,  // 寻找第一航路点
    Auto_Seed_Grip_Seed0,   // 一次取苗
    Auto_Seed_Grip_Seed1,   // 二次取苗
    Auto_Seed_Grip_Seed2,   // 三次取苗
    Auto_Seed_Grip_Seed3,   // 四次取苗
    Auto_Seed_Grip_Seed4,   // 五次取苗
    Auto_Seed_Grip_Seed5,   // 六次取苗
    Auto_Seed_Grip_2Point,  // 寻找第二航路点
    Auto_Seed_Plant_2Point, // 第二航路点旋转
    Auto_Seed_Plant_Seed0,  // 一次放苗
    Auto_Seed_Plant_Seed1,  // 二次放苗
    Auto_Seed_Plant_Seed2,  // 三次放苗
    Auto_Seed_Plant_Seed3,  // 四次放苗
    Auto_Seed_Plant_Seed4,  // 五次放苗
    Auto_Seed_Plant_Seed5,  // 六次放苗
    Auto_Seed_Plant_3Point, // 寻找第三航路点
    Auto_Seed_Plant_4Point, // 第三航路点旋转
};
extern enum Auto_Seed_Point_State auto_seed_point_state;

extern osThreadId_t auto_seed_TaskHandle;

void m_auto_seed_Task_Start(void);

#ifdef __cplusplus
}
#endif
// C++

#endif