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
	#define pin_read(gpio, pin)			( (gpio->IDR & pin) != 0 )
	
	//==============================================================================
	// GPIO pin definitions
	//==============================================================================
	// solenoid control shift register
	#define SOL_SR_GPIO			GPIOG				// the register that is used to shift out data to control the shift registers
	#define SOL_SR_DATA			GPIO_PIN_11			// SER (74hc595 pin 14)	data in
	#define SOL_SR_LATCH		GPIO_PIN_13			// RCK (74hc595 pin 12) register clock (update output)
	#define SOL_SR_CLOCK		GPIO_PIN_12			// SCK (74hc595 pin 11) data clock in
	#define SOL_SR_DIR			1					// direction that the solenoid shift register shifts out key data
	
	// debug pins
	#define DEBUG_GPIO 			GPIOG				// register used for debug pins
	#define DEBUG_0 			GPIO_PIN_0			// pin used for programmer to debug code
	#define DEBUG_1 			GPIO_PIN_1			// pin used for programmer to debug code
	#define DEBUG_WARNING_LED	GPIO_PIN_2			// pin used to turn on an LED to indicate a	warning happened.
	#define DEBUG_ERROR_LED		GPIO_PIN_3			// pin used to turn on an LED to indicate an error	 happened.
	
	// these pins control an input shift-register.
	// The input-shift register is connected to all the digital key input signals.
	// These tells us which key of the xylophone has just been played.
	#define KEY_INPUT_GPIO		GPIOG				// register used for digital key inputs
	#define KEY_INPUT_DATA		GPIO_PIN_10			// pin used to input data from shift registers
	#define KEY_INPUT_LATCH		GPIO_PIN_15			// pin used to latch (shift all bits into register) for input shift-register on input keys
	#define KEY_INPUT_CLOCK		GPIO_PIN_9			// pin used to clock data in bit-by-bit from the input shift-register.
	#define KEY_INPUT_DIR		1					// direction that data is shifted in from the input key shift register.
	
	
	// these pins are inputs/outputs on the control panel
	#define CTRL_IN_ARM_GPIO	GPIOC 
	#define CTRL_IN_ARM			GPIO_PIN_11			// input pin for arming recording mode (recording doesn't start yet, but it will when the user hits a note, or steps on the pedal.
	#define CTRL_IN_PEDAL_GPIO	GPIOC
	#define CTRL_IN_PEDAL		GPIO_PIN_10			// input pin for the pedal. Active LOW. If mode is ARMED, stepping on the pedal will set it to RECORD mode. If in RECORD mode, stepping on pedal exits RECORD mode into PLAY mode).
	#define CTRL_IN_SYNC_GPIO	GPIOH
	#define CTRL_IN_SYNC		GPIO_PIN_2			// input pin for the sync signal (resets the song to timer=0)
	#define CTRL_IN_CAL_GPIO	GPIOD
	#define CTRL_IN_CAL			GPIO_PIN_9			// the calibration button. hold it for a little bit, and the XPhone will go thru a cal. procedure. 
	
	#define CTRL_OUT_LED_R_GPIO	GPIOC
	#define CTRL_OUT_LED_R		GPIO_PIN_12			// output pin controlling red part of the RGB LED
	#define CTRL_OUT_LED_G_GPIO	GPIOD
	#define CTRL_OUT_LED_G		GPIO_PIN_2			// output pin controlling green part of the RGB LED
	#define CTRL_OUT_LED_B_GPIO	GPIOF
	#define CTRL_OUT_LED_B		GPIO_PIN_6			// output pin controlling blue part of the RGB LED
	
	#define CTRL_OUT_SYNC_GPIO	//TODO
	#define CTRL_OUT_SYNC		//TODO			// output pin for syncing other modules. When (SongTime==0), sends out a 1 ms trigger signal (normally 0 V, 3.3 V for 1 ms). 
	
	#define ctrl_LED_o()		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R); pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G); pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = off
	#define ctrl_LED_r()		pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);  pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G); pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = red
	#define ctrl_LED_g()		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R); pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);  pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = green
	#define ctrl_LED_b()		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R); pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G); pin_on(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = blue
	#define ctrl_LED_y()		pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);  pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);  pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = yellow
	#define ctrl_LED_w()		pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);  pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);  pin_on(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = white
	
	
	//==============================================================================
	// GPIO functions
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
	  //__HAL_RCC_GPIOA_CLK_ENABLE();
	  //__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
	  //__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOH_CLK_ENABLE();
		
		// enable solenoid output shift register pins
		GPIO_set_output(SOL_SR_GPIO,SOL_SR_DATA);
		GPIO_set_output(SOL_SR_GPIO,SOL_SR_CLOCK);
		GPIO_set_output(SOL_SR_GPIO,SOL_SR_LATCH);
		
		// enable key input shift register pins
		GPIO_set_input(KEY_INPUT_GPIO,KEY_INPUT_DATA);
		GPIO_set_output(KEY_INPUT_GPIO,KEY_INPUT_CLOCK);
		GPIO_set_output(KEY_INPUT_GPIO,KEY_INPUT_LATCH);
		
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
		
		
		// control panel pins
		GPIO_set_input(CTRL_IN_ARM_GPIO,CTRL_IN_ARM);
		GPIO_set_input(CTRL_IN_CAL_GPIO,CTRL_IN_CAL);
		
		GPIO_set_output(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);
		GPIO_set_output(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);
		GPIO_set_output(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B);
		
		//initialize control pins
		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);
		pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);
		pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B);
		
	}
	
	
	//=============================================================================
	// clock data out of the STM32 into a shift register (serial-in parallel-out shift register such as 74hc595)
	//
	// GPIOx		the port that must be used for all three shift register pins
	// clockPin 	the pin that clocks bits into the shift register
	// dataPin		the pin the STM32 will use to output bits to the shift register
	// latchPin		the pin the STM32 uses to latch the shift register (update the state of all output pins of the shift register at once).
	// bits			the number of bits you want to shift into the register.
	// *data		a pointer to the array (of uint8_t type). The array is treated like a bool (0 or 1). those values are shifted out.
	// dir			the direction of data to be shifted out.
	//				dir=0	=>	data[0],			data[1],			data[2],			...
	//				else	 =>	data[bits-1], data[bits-2], data[bits-3], ...
	//=============================================================================
	void shift_out(GPIO_TypeDef* GPIO, GPIO_Pin_Type clockPin, GPIO_Pin_Type dataPin, GPIO_Pin_Type latchPin, uint32_t bits, uint8_t *data, uint8_t dir)
	{
		uint32_t i;								// used for incrementing through all bits 
		uint32_t b;								// used for indexing the right element of the array (allows us to do MSB first or LSB first)
		for (i = 0; i < bits; i++)
		{
			if (dir)								// if dir is true,
				b = (bits - 1) - i;						// MSB (most significant bit first)
			else									// if dir is false,
				b = i;									// LSB (least significant bit first)
			pin_set(GPIO, dataPin, data[b]);		// output the right data
			pin_off(GPIO, clockPin);				// clock (advance) shift register
			pin_on(GPIO, clockPin);					// ^
		}
		pin_off(GPIO, latchPin);				// update the shift register
		pin_on(GPIO, latchPin);					// ^
		pin_off(GPIO, latchPin);				// ^
	}
	
	
	//=============================================================================
	// clock data into the STM32 from a shift register (parallel-in parallel-out shift register such as 74hc165)
	//
	// GPIOx		the port that must be used for all three shift register pins
	// clockPin 	the pin that clocks bits out of the shift register
	// dataPin		the pin that the STM32 uses for reading bits from the shift register.
	// latchPin		the pin the STM32 uses to latch the shift register (clock all bits into the register at once).
	// bits			the number of bits you want to read in from the shift register.
	// *data		a pointer to the array (of uint8_t type) where the input data will be stored. Boolean values will be stored in this array (0 or 1).
	// dir			the direction of data to be shifted in.
	//				dir=0	=>	data[0],			data[1],			data[2],			...
	//				else	 =>	data[bits-1], data[bits-2], data[bits-3], ...
	//=============================================================================
	void shift_in(GPIO_TypeDef* GPIO, GPIO_Pin_Type clockPin, GPIO_Pin_Type dataPin, GPIO_Pin_Type latchPin, uint32_t bits, uint8_t *data, uint8_t dir)
	{
		pin_off(GPIO,latchPin);					// latch all shift register input bits
		pin_on(GPIO,latchPin);					// ^
		uint32_t i;								// used for incrementing through all bits 
		uint32_t b;								// used for indexing the right element of the array (allows us to do MSB first or LSB first)
		for (i = 0; i < bits; i++)
		{
			if (dir)								// if dir is true,
				b = (bits - 1) - i;						// MSB (most significant bit first)
			else									// if dir is false,
				b = i;									// LSB (least significant bit first)
			data[b] = pin_read(GPIO, dataPin);		// save the input bit in the data[] array.
			pin_off(GPIO, clockPin);				// clock (advance) shift register
			pin_on(GPIO, clockPin);					// ^
		}
	}
	
	
//	void clock_in(GPIO_TypeDef* GPIOx, GPIO_Pin_Type clockPin, GPIO_Pin_Type dataPin, GPIO_Pin_Type latchPin, uint8_t outputSize, uint8_t *data)
//	{
//		uint8_t i, j;
//		uint8_t inData = 0;
//		
//		// cycle the latch to clock all the bits in
//		GPIOx->BSRR = (uint32_t) latchPin << 16U;
//		GPIOx->BSRR = latchPin;
//		
//		for (i = 0; i < outputSize; i++)
//		{
//			for (j = 0; j < 8; j++)
//			{
//				// read data from pin
//				inData = HAL_GPIO_ReadPin(GPIOx, dataPin);
//				
//				// clock data in to get next bit
//				GPIOx->BSRR = (uint32_t) clockPin << 16U;
//				GPIOx->BSRR = clockPin;
//				
//				// insert bits into the output data
//				if (inData != 0) data[i] |= (1 << j);
//			}
//		}
//	}
	
#endif // GPIO_H_INCLUDED
