#include "ashining_as69.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == AS69_UART) {
        wtrMavlink_UARTRxCpltCallback(huart, MAVLINK_COMM_0);
    }
}

void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    switch (msg->msgid) {
        case 209:
            mavlink_msg_joystick_air_decode(msg, &msg_joystick_air);
            break;
        default:
            break;
    }
}