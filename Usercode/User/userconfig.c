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