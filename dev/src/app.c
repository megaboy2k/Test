/*
 * app.cpp
 *
 *  Created on: 18 Jun 2024
 *      Author: Sergii Klymenko
 *
 *  Description:    UART <-> SPI gateway
 *                  Peripheral transactions are done using interrupts.
 *                  UART TX and handler utilizes RTOS semaphore to synchronize with the thread.
 *                  UART RX handler uses thread-safe FIFO buffer to hand-over data from handler to thread
 *                  with buffering.
 *                  SPI transaction uses its RX complete interrupt to synchronize with the following
 *                  UART start transmission.
 *                  The whole sequence is based on UART TX complete interrupt event so the maximum
 *                  throughput is stuck to UART baudrate and is 115200.
 */

#include <stdio.h>

#include "cmsis_os.h"
#include "main.h"
#include "common.h"
#include "utils.h"
#include "fifo.h"
#include "app.h"

#define FIFO_LEN    16

static void SpiRxCpltCallback( SPI_HandleTypeDef *hspi );
static void UartRxByteCallback( UART_HandleTypeDef *huart, uint16_t rx_byte );
static void UartTxCpltCallback( UART_HandleTypeDef *huart );
static void SpiCommBegin();
static void SpiCommEnd();

static HAL_Handlers_t *hal;
static osSemaphoreId sema;
static struct fifo_buffer fifo;
static uint8_t fifo_buf[FIFO_LEN];

/*! @brief  Entry point to the user application after CubeMX initialization
 *  @param  hal - peripheral handles
 *  @return none
 */
void app_task( HAL_Handlers_t *_hal )
{
    HAL_StatusTypeDef status;
    osStatus os_rc;
    uint8_t value;
    uint8_t spi_rx_data;

    sema = osSemaphoreCreate( NULL, 1 );
    osSemaphoreWait( sema, 0 ); // set semaphore to initial value

    MicrosecondsInitialize( hal->usec_htim );
    fifo_init(&fifo, fifo_buf, FIFO_LEN);

    hal = _hal;
    HAL_SPI_RegisterCallback( hal->hspi, HAL_SPI_RX_COMPLETE_CB_ID, SpiRxCpltCallback );
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

        SpiCommBegin();
        status = HAL_SPI_TransmitReceive_IT( hal->hspi, &value, &spi_rx_data, 1 );
        ASSERT( status == HAL_OK );
        os_rc = osSemaphoreWait( sema, osWaitForever );
        ASSERT( os_rc == osOK );

        DelayMicroseconds( 10 ); // tSCLK-NCS delay
        SpiCommEnd();

        status = HAL_UART_Transmit_IT( hal->huart, &spi_rx_data, 1 );
        ASSERT( status == HAL_OK );
    }
}

void SpiRxCpltCallback( SPI_HandleTypeDef *hspi )
{
    osSemaphoreRelease( sema );
}

void UartRxByteCallback( UART_HandleTypeDef *huart, uint16_t rx_byte )
{
    fifo_write(&fifo, (uint8_t *)&rx_byte, 1);
}

void UartTxCpltCallback( UART_HandleTypeDef *huart )
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

