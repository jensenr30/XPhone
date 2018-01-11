#ifndef UART_H_DEF
	#define UART_H_DEF
	
	#include "stm32f4xx_hal.h"
	#include "stm32f4xx_nucleo_144.h"
	#include "stdio.h"
		
	#define USARTx							USART3
	#define USARTx_CLK_ENABLE()				__HAL_RCC_USART3_CLK_ENABLE();
	#define USARTx_RX_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOD_CLK_ENABLE()
	#define USARTx_TX_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOD_CLK_ENABLE()
	
	#define USARTx_FORCE_RESET()			__HAL_RCC_USART3_FORCE_RESET()
	#define USARTx_RELEASE_RESET()			__HAL_RCC_USART3_RELEASE_RESET()
	
	/* Definition for USARTx Pins */
	#define USARTx_TX_PIN					GPIO_PIN_8
	#define USARTx_TX_GPIO_PORT				GPIOD
	#define USARTx_TX_AF					GPIO_AF7_USART3
	#define USARTx_RX_PIN					GPIO_PIN_9
	#define USARTx_RX_GPIO_PORT				GPIOD
	#define USARTx_RX_AF					GPIO_AF7_USART3
	
	#define USARTx_BUAD						9600
	#define newline							"\n"
	
	UART_HandleTypeDef UartHandle;
	
	//--------------------------------------------------------------------------
	// redefine the PUTCHAR_PROTOTYPE. This allows us to use printf() to send bytes to the UART.
	//--------------------------------------------------------------------------
	#ifdef __GNUC__
	/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
	#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
	#endif /* __GNUC__ */
	PUTCHAR_PROTOTYPE;
	
	void UART_print(char *);
	void UART_init();
	
#endif // UART_H_DEF
