/**
 * @file SerlCan_Intrpt.h
 * @author Junfeng Shen (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __SERLCAN_INTRPT_H__
#define __SERLCAN_INTRPT_H__
#include "stm32f1xx_hal.h"
#include "StdTypeDef.h"
#include "SerlCan.h"


extern CAN_HandleTypeDef hcan;
extern UART_HandleTypeDef huart1;



extern void SerlCan_SendData(SerlCan_Arch* CanData);
extern void SerlCan_SendDataToUart(void);

#endif /*__SERLCAN_INTRPT_H__*/
