/*
 * utils.h
 *
 *  Created on: 10 Jun 2022
 *      Author: Sergii Klymenko
 */

#ifndef SRC_UTILS_UTILS_H_
#define SRC_UTILS_UTILS_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "stm32f0xx_hal.h"


/*!
 * \class   Provides helper functions
 */
void MicrosecondsInitialize( TIM_HandleTypeDef *htim ); // should be called on application start
uint32_t MicrosecondsGet(); // Get microsecond timer value
void DelayMicroseconds( uint16_t value );


#endif /* SRC_UTILS_UTILS_H_ */
