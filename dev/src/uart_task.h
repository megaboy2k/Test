/*
 * uart_task.h
 *
 *  Created on: 19 Jun 2022
 *      Author: Sergii Klymenko
 */

#ifndef SRC_UART_TASK_H_
#define SRC_UART_TASK_H_

#ifdef  __cplusplus
extern "C" {
#endif

void uart_task( void const *_arg );
void uart_sendData( uint8_t value );

#ifdef  __cplusplus
}
#endif

#endif /* SRC_UART_TASK_H_ */
