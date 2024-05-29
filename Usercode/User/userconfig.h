#ifndef __USERCONFIG_H
#define __USERCONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

#include "usermain.h"

#define AS69_UART_HANDLE      huart1
#define AS69_UART             USART1

#define USE_FREERTOS          1

#define ID_DIRECT             1
#define ID_HANDLE_SEED        2
#define ID_HANDLE_BALL        2
#define ID_AUTO_SEED          2
#define ID_AUTO_BALL          2
#define ID_DIRECT_CHOOSE      2
#define ID_RUN                3
#define ID_MODE               4

#define chassis_R             (152 * 0.001) // 全向轮半径
#define chassis_r             (600 * 0.001) // 底盘半径
#define PI                    3.1415926
#define Reduction_ratio       (3591.0f / 187.0f) // M3508减速比

extern mavlink_joystick_air_dashboard_set_title_t mav_dir_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_dir_msg;
extern char direct_title[20];
extern char direct_left_msg[20];
extern char direct_right_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_run_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_run_msg;
extern char run_title[20];
extern char run_auto_msg[20];
extern char run_handle_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_dir_choose_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_dir_choose_msg;
extern char direct_choose_title[20];
extern char direct_choose_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_mode_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_mode_msg;
extern char mode_title[20];
extern char mode_idle_msg[20];
extern char mode_seed_msg[20];
extern char mode_ball_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_handle_seed_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_handle_seed_msg;
extern char handle_seed_title[20];
extern char handle_seed_ready_msg[20];
extern char handle_seed_grip_msg[20];
extern char handle_seed_plant_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_handle_ball_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_handle_ball_msg;
extern char handle_ball_title[20];
extern char handle_ball_ready_msg[20];
extern char handle_ball_pick_msg[20];
extern char handle_ball_fire_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_auto_seed_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_auto_seed_msg;
extern char auto_seed_title[20];
extern char auto_seed_ready_msg[20];
extern char auto_seed_grip_msg[20];
extern char auto_seed_plant_msg[20];

extern mavlink_joystick_air_dashboard_set_title_t mav_auto_ball_title;
extern mavlink_joystick_air_dashboard_set_msg_t mav_auto_ball_msg;
extern char auto_ball_title[20];
extern char auto_ball_ready_msg[20];
extern char auto_ball_go_msg[20];
extern char auto_ball_fire_msg[20];

#ifdef __cplusplus
}
#endif

#endif