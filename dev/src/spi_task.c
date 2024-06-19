/*
 * spi_task.c
 *
 *  Created on: 19 Jun 2024
 *      Author: Sergii Klymenko
 */

#include <stdio.h>

#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "common.h"
#include "utils.h"
#include "fifo.h"
#include "uart_task.h"
#include "app.h"

#define MQ_SPI_MAX_SIZE     10

static void SpiRxCpltCallback( SPI_HandleTypeDef *hspi );
static void SpiCommBegin();
static void SpiCommEnd();

osMailQDef( spi_mq, MQ_SPI_MAX_SIZE, uint8_t );

static HAL_Handlers_t *hal;
static osSemaphoreId sema;
static osMailQId queue;

/*! @brief  SPI thread function
 *  @param  _arg - shared parameters
 *  @return none
 */
void spi_task( void const *_arg )
{
    HAL_StatusTypeDef status;
    osStatus os_rc;
    osEvent rv;
    uint8_t value;
    uint8_t spi_rx_data;

    hal = (HAL_Handlers_t *)_arg;
    queue = osMailCreate( osMailQ(spi_mq), NULL );
    sema = osSemaphoreCreate( NULL, 1 );
    osSemaphoreWait( sema, 0 ); // set semaphore to initial value

    HAL_SPI_RegisterCallback( hal->hspi, HAL_SPI_RX_COMPLETE_CB_ID, SpiRxCpltCallback );

    while( 1 )
    {
        rv = osMailGet( queue, osWaitForever );
        ASSERT( rv.status == osEventMail );

        value = rv.value.v; // ascii or zero

        // SPI transaction is much faster then UART so we can do it just in place
        SpiCommBegin();
        status = HAL_SPI_TransmitReceive_IT( hal->hspi, &value, &spi_rx_data, 1 );
        ASSERT( status == HAL_OK );
        os_rc = osSemaphoreWait( sema, osWaitForever );
        ASSERT( os_rc == osOK );
        DelayMicroseconds( 10 ); // tSCLK-NCS delay
        SpiCommEnd();

        uart_sendData( spi_rx_data );
    }
}

void spi_sendData( uint8_t value )
{
    osMailPut( queue, &value );
}

void SpiRxCpltCallback( SPI_HandleTypeDef *hspi )
{
    osSemaphoreRelease( sema );
}

void SpiCommBegin()
{
    HAL_GPIO_WritePin( NCS_GPIO_Port, NCS_Pin, GPIO_PIN_RESET );
}

void SpiCommEnd()
{
    HAL_GPIO_WritePin( NCS_GPIO_Port, NCS_Pin, GPIO_PIN_SET );
}

