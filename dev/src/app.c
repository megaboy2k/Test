/*
 * app.c
 *
 *  Created on: 18 Jun 2024
 *      Author: Sergii Klymenko
 *
 *  Description:    UART <-> SPI gateway
 *                  Peripheral transactions are done using interrupts.
 *                  UART TX IRQ handler utilizes RTOS semaphore to synchronize with the thread.
 *                  UART RX IRQ handler uses thread-safe FIFO buffer to hand-over data from IRQ to thread
 *                  with buffering.
 *                  SPI transaction uses its RX complete interrupt to synchronize with the following
 *                  UART start transmission.
 *                  The whole sequence is based on UART TX complete interrupt timing so the maximum
 *                  throughput is stuck to UART baudrate and is 115200.
 *                  SPI baudrate is 1.5 Mbits
 *
 *                  UART and SPI handlers are separated to their own threads. Inter-thread handshake
 *                  is done using message queues
 */

#include <stdio.h>

#include "cmsis_os.h"
#include "main.h"
#include "common.h"
#include "utils.h"
#include "fifo.h"
#include "uart_task.h"
#include "spi_task.h"
#include "app.h"

// Note, that size of stack is in sizeof(portBASE_TYPE)
#define TASK_UART_STACK_SIZE     (256)
#define TASK_SPI_STACK_SIZE      (256)

osThreadDef(UartTask,    uart_task,   osPriorityNormal,       0,  TASK_UART_STACK_SIZE);
osThreadDef(SpiTask,     spi_task,    osPriorityNormal,       0,  TASK_SPI_STACK_SIZE);

/*! @brief  Entry point to the user application after CubeMX initialization
 *  @param  hal - peripheral handles
 *  @return none
 */
void app_task( HAL_Handlers_t *_hal )
{
    MicrosecondsInitialize( _hal->usec_htim );

    osThreadCreate(osThread(UartTask), _hal);
    osThreadCreate(osThread(SpiTask), _hal);

    while( 1 )
    {
        // Any background activity
        osDelay( 1000 );
    }
}

