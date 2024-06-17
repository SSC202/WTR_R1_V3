#include "userconfig.h"

/*****************************************************************************
 * 消息定义
 */

mavlink_joystick_air_dashboard_set_title_t mav_dir_title;
mavlink_joystick_air_dashboard_set_msg_t mav_dir_msg;
char direct_title[20]     = "Direct";
char direct_left_msg[20]  = "LeftMode";
char direct_right_msg[20] = "RightMode";

mavlink_joystick_air_dashboard_set_title_t mav_run_title;
mavlink_joystick_air_dashboard_set_msg_t mav_run_msg;
char run_title[20]      = "Run";
char run_auto_msg[20]   = "AutoMode";
char run_handle_msg[20] = "HandleMode";

mavlink_joystick_air_dashboard_set_title_t mav_dir_choose_title;
mavlink_joystick_air_dashboard_set_msg_t mav_dir_choose_msg;
char direct_choose_title[20] = "DirectChoose";
char direct_choose_msg[20]   = "Waiting";

mavlink_joystick_air_dashboard_set_title_t mav_mode_title;
mavlink_joystick_air_dashboard_set_msg_t mav_mode_msg;
char mode_title[20]    = "Mode";
char mode_idle_msg[20] = "IDLEMode";
char mode_seed_msg[20] = "SeedMode";
char mode_ball_msg[20] = "BallMode";

mavlink_joystick_air_dashboard_set_title_t mav_handle_seed_title;
mavlink_joystick_air_dashboard_set_msg_t mav_handle_seed_msg;
char handle_seed_title[20]     = "SeedWork";
char handle_seed_ready_msg[20] = "Ready";
char handle_seed_grip_msg[20]  = "Grip";
char handle_seed_plant_msg[20] = "Plant";

mavlink_joystick_air_dashboard_set_title_t mav_handle_ball_title;
mavlink_joystick_air_dashboard_set_msg_t mav_handle_ball_msg;
char handle_ball_title[20]     = "BallWork";
char handle_ball_ready_msg[20] = "Ready";
char handle_ball_pick_msg[20]  = "Pick";
char handle_ball_fire_msg[20]  = "Fire";

mavlink_joystick_air_dashboard_set_title_t mav_auto_seed_title;
mavlink_joystick_air_dashboard_set_msg_t mav_auto_seed_msg;
char auto_seed_title[20]     = "SeedWork";
char auto_seed_ready_msg[20] = "Ready";
char auto_seed_grip_msg[20]  = "Grip";
char auto_seed_plant_msg[20] = "Plant";

mavlink_joystick_air_dashboard_set_title_t mav_auto_ball_title;
mavlink_joystick_air_dashboard_set_msg_t mav_auto_ball_msg;
char auto_ball_title[20]     = "BallWork";
char auto_ball_ready_msg[20] = "Ready";
char auto_ball_go_msg[20]    = "Going";
char auto_ball_fire_msg[20]  = "Fire";

/*****************************************************************************
 * 点位定义
 */
float left_grip_seed_point_x[6]  = {185, 185, 185, 185, 185, 185};             // 左侧取苗纵向坐标
float left_grip_seed_point_y[6]  = {-1240, -1490, -2240, -2490, -3240, -3490}; // 左侧取苗横向坐标
float left_plant_seed_point_x[6] = {2273, 2273, 2273, 2273, 2273, 2273};       // 左侧放苗纵向坐标
float left_plant_seed_point_y[6] = {-3193, -2693, -2193, -1693, -1193, -693};  // 左侧放苗横向坐标
float left_ball_point_x[12]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};       // 左侧球纵向坐标
float left_ball_point_y[12]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};       // 左侧球横向坐标

float right_grip_seed_point_x[6]  = {185, 185, 185, 185, 185, 185};       // 右侧取苗纵向坐标
float right_grip_seed_point_y[6]  = {1240, 1490, 2240, 2490, 3240, 3490}; // 右侧取苗横向坐标
float right_plant_seed_point_x[6] = {2273, 2273, 2273, 2273, 2273, 2273}; // 右侧放苗纵向坐标
float right_plant_seed_point_y[6] = {3193, 2693, 2193, 1693, 1193, 693};  // 右侧放苗横向坐标
float right_ball_point_x[12]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 右侧球纵向坐标
float right_ball_point_y[12]      = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 右侧球横向坐标