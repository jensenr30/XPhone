#ifndef GPIO_H_INCLUDED
	#define GPIO_H_INCLUDED
	#include "stm32f4xx.h"
	#include "stm32f4xx_nucleo_144.h"
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
	#define SOL_SR_DATA_GPIO	GPIOG				//
	#define SOL_SR_DATA			GPIO_PIN_11			// SER (74hc595 pin 14)	data in
	#define SOL_SR_LATCH_GPIO	GPIOG				//
	#define SOL_SR_LATCH		GPIO_PIN_13			// RCK (74hc595 pin 12) register clock (update output)
	#define SOL_SR_CLOCK_GPIO	GPIOG				//
	#define SOL_SR_CLOCK		GPIO_PIN_12			// SCK (74hc595 pin 11) data clock in
	#define SOL_SR_DIR			1					// direction that the solenoid shift register shifts out key data

	// ANALOG MULTIPLEXER shift register: these pins control a shift register.
	// The outputs of the shift register drive the logic inputs of the analog multiplexer.
	// this allows me to select which which key's amplitude I want to sample with the ADC.
	#define AMUX_SR_DATA_GPIO	GPIOD				// 
	#define AMUX_SR_DATA		GPIO_PIN_1			// used to output data to the shift register.
	#define AMUX_SR_CLOCK_GPIO	GPIOF				// 
	#define AMUX_SR_CLOCK		GPIO_PIN_8			// used to clock said data into the shift register.
	
	// debug pins
	#define DEBUG_0_GPIO			GPIOG			
	#define DEBUG_0					GPIO_PIN_2		// used for whatever the fuck I feel like.   :-)
	#define DEBUG_LED_GPIO			GPIOF
	#define DEBUG_LED				GPIO_PIN_9		// used for whatever the fuck I feel like.   :-)
	#define DEBUG_WARNING_LED_GPIO 	GPIOE			
	#define DEBUG_WARNING_LED		GPIO_PIN_1		// used to turn on an LED to indicate the warning() function was called.
	#define DEBUG_ERROR_LED_GPIO	GPIOE			
	#define DEBUG_ERROR_LED			GPIO_PIN_6		// used to turn on an LED to indicate the error() function was called.
	
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
	// todo assign a pin to the cal gpio
	#define CTRL_IN_CAL_GPIO	GPIOD
	#define CTRL_IN_CAL			GPIO_PIN_9			// the calibration button. hold it for a little bit, and the XPhone will go thru a cal. procedure. 
	#define CTRL_IN_CLEAR_GPIO	GPIOG
	#define CTRL_IN_CLEAR		GPIO_PIN_0			// pin used to clear the current song 
	#define CTRL_IN_TIMING_GPIO	GPIOG				
	#define CTRL_IN_TIMING		GPIO_PIN_1			// pin checks to see what timing we should use. if pin is high, we use external timing (song resets when CTRL_IN_SYNC signal goes high). low=internal timing (song resets when SongTime == SongLength).
	#define CTRL_IN_SYNC_GPIO	GPIOD				
	#define CTRL_IN_SYNC		GPIO_PIN_0			// if the user selects external timing, this pin will control when the XPhone resets the SongTime to 0.

	#define CTRL_OUT_LED_R_GPIO	GPIOC
	#define CTRL_OUT_LED_R		GPIO_PIN_12			// output pin controlling red part of the RGB LED
	#define CTRL_OUT_LED_G_GPIO	GPIOD
	#define CTRL_OUT_LED_G		GPIO_PIN_2			// output pin controlling green part of the RGB LED
	#define CTRL_OUT_LED_B_GPIO	GPIOF
	#define CTRL_OUT_LED_B		GPIO_PIN_6			// output pin controlling blue part of the RGB LED
	
	//#define CTRL_OUT_SYNC_GPIO	//TODO
	//#define CTRL_OUT_SYNC		//TODO			// output pin for syncing other modules. When (SongTime==0), sends out a 1 ms trigger signal (normally 0 V, 3.3 V for 1 ms). 
	
	#define ctrl_LED_o()		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R); pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G); pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = off
	#define ctrl_LED_r()		pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);  pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G); pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = red
	#define ctrl_LED_g()		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R); pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);  pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = green
	#define ctrl_LED_b()		pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R); pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G); pin_on(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = blue
	#define ctrl_LED_y()		pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);  pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);  pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = yellow
	#define ctrl_LED_w()		pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);  pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);  pin_on(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B)	// led = white
	
	
	//==============================================================================
	// GPIO functions
	//==============================================================================
	void GPIO_set_output(GPIO_TypeDef *GPIO,uint16_t Pin);
	void GPIO_set_input(GPIO_TypeDef *GPIO, uint16_t Pin);
	void GPIO_init();
	void shift_out(GPIO_TypeDef* clockGPIO, GPIO_Pin_Type clockPin, GPIO_TypeDef* dataGPIO, GPIO_Pin_Type dataPin, GPIO_TypeDef* latchGPIO, GPIO_Pin_Type latchPin, uint32_t bits, uint8_t *data, uint8_t dir);
	void shift_in(GPIO_TypeDef* GPIO, GPIO_Pin_Type clockPin, GPIO_Pin_Type dataPin, GPIO_Pin_Type latchPin, uint32_t bits, uint8_t *data, uint8_t dir);
	
#endif // GPIO_H_INCLUDED
