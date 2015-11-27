#ifndef __UART_H
#define __UART_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "ticks.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef enum
{
	COM_NULL = -1,  // disabled
	COM1 = 0,        // usart1
	COM3 = 1,       // usart3
} COM_TypeDef;

#define COMn 3

// Definition for USART1
#define COM1_CLK                    RCC_APB2Periph_USART1
#define COM1_TX_PIN                 GPIO_Pin_9
#define COM1_TX_GPIO_PORT           GPIOA
#define COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define COM1_RX_PIN                 GPIO_Pin_10
#define COM1_RX_GPIO_PORT           GPIOA
#define COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define COM1_IRQn                   USART1_IRQn

// Definition for USART3
#define COM3_CLK                    RCC_APB1Periph_USART3
#define COM3_TX_PIN                 GPIO_Pin_10
#define COM3_TX_GPIO_PORT           GPIOB
#define COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define COM3_RX_PIN                 GPIO_Pin_11
#define COM3_RX_GPIO_PORT           GPIOB
#define COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define COM3_IRQn                   USART3_IRQn

extern USART_TypeDef* COM_USART[COMn];
extern COM_TypeDef printf_COMx;

// COM1=USART1, COM3=UART3
void uart_init(COM_TypeDef COM, u32 br);
void uart_interrupt(COM_TypeDef COM);

void uart_printf_enable(COM_TypeDef COM);
void uart_printf_disable(void);

void uart_tx_byte(COM_TypeDef COM, const uint8_t data);
void uart_tx(COM_TypeDef COM, const uint8_t * tx_buf, ...);

uint8_t uart_rx_byte(COM_TypeDef COM);

typedef void on_receive_listener (const uint8_t byte);
void uart_interrupt_init(COM_TypeDef COM, on_receive_listener *listener);

/************************************************************************************************************/
#define FIFO_BUFFER_SIZE 128 // software buffer size (in bytes)

// UART data receive function
//  - checks if data exists in the receive sw buffer
//  - if data exists, it returns the oldest element contained in the buffer
//  - automatically handles "uart_rx_buffer_full_flag"
//  - if no data exists, it clears the uart_rx_flag
uint8_t uart_get_byte(void);

extern volatile uint8_t uart_rx_fifo_not_empty_flag; // this flag is automatically set and cleared by the software buffer
extern volatile uint8_t uart_rx_fifo_full_flag;      // this flag is automatically set and cleared by the software buffer
extern volatile uint8_t uart_rx_fifo_ovf_flag;

void listener(uint8_t);

uint8_t uart_get_byte(void);

#endif  /* __UART_H */
