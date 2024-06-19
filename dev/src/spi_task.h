/*
 * spi_task.h
 *
 *  Created on: 19 Jun 2022
 *      Author: Sergii Klymenko
 */

#ifndef SRC_SPI_TASK_H_
#define SRC_SPI_TASK_H_

#ifdef  __cplusplus
extern "C" {
#endif

void spi_task( void const *_arg );
void spi_sendData( uint8_t value );

#ifdef  __cplusplus
}
#endif

#endif /* SRC_SPI_TASK_H_ */
