#ifndef GPIO_H_INCLUDED
	#define GPIO_H_INCLUDED
	#include "stm32f4xx.h"
	#include "stm32f4xx_nucleo_144.h"
	static GPIO_InitTypeDef GPIO_Struct;
	#define GPIO_Pin_Type uint16_t
	
	//==============================================================================
	// GPIO pin control functions
	//==============================================================================
	#define pin_set(gpio, pin, state)	if(state) gpio->ODR |= (pin); else gpio->ODR &= ~(pin)
	#define pin_on(gpio, pin)			gpio->ODR |= (pin)
	#define pin_off(gpio, pin)			gpio->ODR &= ~(pin)
	#define pin_tog(gpio, pin)			gpio->ODR ^= (pin)
	
	//==============================================================================
	// GPIO pin definitions
	//==============================================================================
	// solenoid control shift register
	#define SOL_SR_GPIO			GPIOC				// the register that is used to shift out data to control the shift registers
	#define SOL_SR_DATA			GPIO_PIN_1			// SER (74hc595 pin 14)	data in
	#define SOL_SR_LATCH		GPIO_PIN_4			// RCK (74hc595 pin 12) register clock (update output)
	#define SOL_SR_CLOCK		GPIO_PIN_5			// SCK (74hc595 pin 11) data clock in
	#define SOL_SR_DIR 1							// direction that the solenoid shift register shifts out key data
	
	// debug pins
	#define DEBUG_GPIO 			GPIOG				// register used for debug pins
	#define DEBUG_0 			GPIO_PIN_0			// pin used for programmer to debug code
	#define DEBUG_1 			GPIO_PIN_1			// pin used for programmer to debug code
	#define DEBUG_WARNING_LED	GPIO_PIN_2			// pin used to turn on an LED to indicate a	warning happened.
	#define DEBUG_ERROR_LED		GPIO_PIN_3			// pin used to turn on an LED to indicate an error	 happened.
	
	// these pins control an input shift-register.
	// The input-shift register is connected to all the digital key input signals.
	// These tells us which key of the xylophone has just been played.
	#define KEY_INPUT_GPIO		GPIOF				// register used for digital key inputs
	#define KEY_INPUT_DATA		GPIO_PIN_2			// pin used to input data from shift registers
	#define KEY_INPUT_LATCH		GPIO_PIN_1			// pin used to latch (shift all bits into register) for input shift-register on input keys
	#define KEY_INPUT_CLOCK		GPIO_PIN_0			// pin used to clock data in bit-by-bit from the input shift-register.
	
	//==============================================================================
	// GPIO pins init function
	//==============================================================================
	
	
	// set a pin to an output
	void GPIO_set_output(GPIO_TypeDef *GPIO,uint16_t Pin)
	{
		GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_Struct.Pull = GPIO_NOPULL;
		GPIO_Struct.Speed = GPIO_SPEED_HIGH;
		GPIO_Struct.Pin = Pin;
		HAL_GPIO_Init(GPIO, &GPIO_Struct);
	}
	
	// set a pin to an input
	void GPIO_set_input(GPIO_TypeDef *GPIO, uint16_t Pin)
	{
		GPIO_Struct.Mode = GPIO_MODE_INPUT;
		GPIO_Struct.Pull = GPIO_NOPULL;
		GPIO_Struct.Speed = GPIO_SPEED_HIGH;
		GPIO_Struct.Pin = Pin;
		HAL_GPIO_Init(GPIO, &GPIO_Struct);
	}
	
	// init all the pins the STM32 will use.
	void GPIO_init()
	{
		// enable the clocks for the ports we want to use
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		
		// enable solenoid output shift register pins
		GPIO_set_output(SOL_SR_GPIO,SOL_SR_DATA);
		GPIO_set_output(SOL_SR_GPIO,SOL_SR_CLOCK);
		GPIO_set_output(SOL_SR_GPIO,SOL_SR_LATCH);
		
		// enable debug pins
		GPIO_set_output(DEBUG_GPIO,DEBUG_0);
		GPIO_set_output(DEBUG_GPIO,DEBUG_1);
		GPIO_set_output(DEBUG_GPIO,DEBUG_WARNING_LED);
		GPIO_set_output(DEBUG_GPIO,DEBUG_ERROR_LED);
		// initialize debug pins
		pin_off(DEBUG_GPIO, DEBUG_0);
		pin_off(DEBUG_GPIO, DEBUG_1);
		pin_off(DEBUG_GPIO, DEBUG_WARNING_LED);
		pin_off(DEBUG_GPIO, DEBUG_ERROR_LED);
		
//		// enable input shift register
//		GPIO_Struct.Pin = KEY_INPUT_LATCH; HAL_GPIO_Init(GPIOF, &GPIO_Struct);
//		GPIO_Struct.Pin = KEY_INPUT_CLOCK; HAL_GPIO_Init(GPIOF, &GPIO_Struct);
//		GPIO_Struct.Mode = GPIO_MODE_INPUT;
//		GPIO_Struct.Pin = KEY_INPUT_DATA; HAL_GPIO_Init(GPIOF, &GPIO_Struct);
//		// set latch pin to high
//		GPIOF->BSRR = KEY_INPUT_LATCH;
		
	}
	
	//=============================================================================
	// clock data out of the STM32 into a shift register (serial-in parallel-out shift register such as 74hc595)
	//
	// GPIOx		the port that must be used for all three shift register pins
	// clockPin 	the pin that clocks data into the shift register
	// dataPin		the data pin that the shift register will be inputting.
	// latchPin		the pin that updates the register's output pins when all the data has been shifted into it.
	// bits			the number of bits you want to shift into the register.
	// *data		a pointer to the array (of uint8_t type). The array is treated like a bool (0 or 1). those values are shifted out.
	// dir			the direction of data to be shifted out.
	//				dir=0	=>	data[0],			data[1],			data[2],			...
	//				else	 =>	data[bits-1], data[bits-2], data[bits-3], ...
	//=============================================================================
	void shift_out(GPIO_TypeDef* GPIO, GPIO_Pin_Type clockPin, GPIO_Pin_Type dataPin, GPIO_Pin_Type latchPin, uint32_t bits, uint8_t *data, uint8_t dir)
	{
		uint32_t b;
		uint32_t i;
		for (i = 0; i < bits; i++)
		{
			if (dir)
			{
				b = (bits - 1) - i;
			}
			else
			{
				b = i;
			}
			// output the right data
			pin_set(GPIO, dataPin, data[b]);
			// clock data in
			pin_off(GPIO, clockPin);
			pin_on(GPIO, clockPin);
		}
		// update the shift register
		pin_off(GPIO, latchPin);
		pin_on(GPIO, latchPin);
		pin_off(GPIO, latchPin);
	}
	
	void clock_in(GPIO_TypeDef* GPIOx, GPIO_Pin_Type clockPin, GPIO_Pin_Type dataPin, GPIO_Pin_Type latchPin, uint8_t outputSize, uint8_t *data)
	{
		uint8_t i, j;
		uint8_t inData = 0;
		
		// cycle the latch to clock all the bits in
		GPIOx->BSRR = (uint32_t) latchPin << 16U;
		GPIOx->BSRR = latchPin;
		
		for (i = 0; i < outputSize; i++)
		{
			for (j = 0; j < 8; j++)
			{
				// read data from pin
				inData = HAL_GPIO_ReadPin(GPIOx, dataPin);
				
				// clock data in to get next bit
				GPIOx->BSRR = (uint32_t) clockPin << 16U;
				GPIOx->BSRR = clockPin;
				
				// insert bits into the output data
				if (inData != 0) data[i] |= (1 << j);
			}
		}
	}
	
#endif // GPIO_H_INCLUDED
