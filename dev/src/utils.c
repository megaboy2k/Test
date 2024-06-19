/*
 * utils.c
 *
 *  Created on: 10 Jun 2022
 *      Author: Sergii Klymenko
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "utils.h"


// This timer must be configured to 1 uS counting step
static TIM_HandleTypeDef * g_htim;


//*****************************************************************************
void MicrosecondsInitialize( TIM_HandleTypeDef *htim )
{
    g_htim = htim;
    HAL_TIM_Base_Start_IT( g_htim );
}


uint32_t MicrosecondsGet()
{
    return __HAL_TIM_GET_COUNTER( g_htim );
}


void DelayMicroseconds( uint16_t value )
{
    uint16_t start = MicrosecondsGet();

    while( (uint16_t)((uint16_t)MicrosecondsGet() - start) < value );
}

