/*
 * app.h
 *
 *  Created on: 18 Jun 2022
 *      Author: Sergii Klymenko
 */

#ifndef SRC_APP_H_
#define SRC_APP_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"

#define SW_VERSION_HIGH         0
#define SW_VERSION_LOW          1

// Version history:
// 0.1 -    Base version


//! @struct Peripheral handles after CubeMX initialization
typedef struct
{
    TIM_HandleTypeDef *     usec_htim;      // 1 us timer
    UART_HandleTypeDef *    huart;          // gateway UART side
    SPI_HandleTypeDef *     hspi;           // gateway SPI side

} HAL_Handlers_t;


void app_task( HAL_Handlers_t *_hal );


#ifdef  __cplusplus
}
#endif

#endif /* SRC_APP_H_ */
