/*
 * uart_task.c
 *
 *  Created on: 19 Jun 2024
 *      Author: Sergii Klymenko
 */

#include <stdio.h>

#include "cmsis_os.h"
#include "main.h"
#include "common.h"
#include "utils.h"
#include "fifo.h"
#include "spi_task.h"
#include "app.h"

#define FIFO_LEN            16
#define MQ_UART_MAX_SIZE    10

static void UartRxByteCallback( UART_HandleTypeDef *huart, uint16_t rx_byte );
static void UartTxCpltCallback( UART_HandleTypeDef *huart );

osMailQDef( uart_mq, MQ_UART_MAX_SIZE, uint8_t );

static HAL_Handlers_t *hal;
static osSemaphoreId sema;
static osMailQId queue;
static struct fifo_buffer fifo;
static uint8_t fifo_buf[FIFO_LEN];

/*! @brief  UART thread function
 *  @param  _arg - shared parameters
 *  @return none
 */
void uart_task( void const *_arg )
{
    HAL_StatusTypeDef status;
    osStatus os_rc;
    osEvent rv;
    uint8_t value;
    uint8_t spi_rx_data;

    hal = (HAL_Handlers_t *)_arg;
    queue = osMailCreate( osMailQ(uart_mq), NULL );
    sema = osSemaphoreCreate( NULL, 1 );
    osSemaphoreWait( sema, 0 ); // set semaphore to initial value

    fifo_init(&fifo, fifo_buf, FIFO_LEN);

    HAL_UART_RegisterCallback( hal->huart, HAL_UART_TX_COMPLETE_CB_ID, UartTxCpltCallback );
    hal->huart->RxByteCallback = UartRxByteCallback;

    HAL_UART_ReceiveEnable_IT( hal->huart );
    value = 0;
    // start main loop
    status = HAL_UART_Transmit_IT( hal->huart, &value, 1 );
    ASSERT( status == HAL_OK );

    while( 1 )
    {
        os_rc = osSemaphoreWait( sema, osWaitForever );
        ASSERT( os_rc == osOK );
        uint32_t cnt = fifo_read(&fifo, &value, 1);
        if( !cnt )
        {
            value = 0;
        }

        spi_sendData( value );

        rv = osMailGet( queue, 0 );
        ASSERT( rv.status == osEventMail || rv.status == osOK );
        if( rv.status == osEventMail )
        {
            spi_rx_data = rv.value.v; // ascii or zero
        }
        else
        {
            spi_rx_data = 0;
        }

        status = HAL_UART_Transmit_IT( hal->huart, &spi_rx_data, 1 );
        ASSERT( status == HAL_OK );
    }
}

void uart_sendData( uint8_t value )
{
    osMailPut( queue, &value );
}

void UartRxByteCallback( UART_HandleTypeDef *huart, uint16_t rx_byte )
{
    fifo_write(&fifo, (uint8_t *)&rx_byte, 1);
}

void UartTxCpltCallback( UART_HandleTypeDef *huart )
{
    osSemaphoreRelease( sema );
}



