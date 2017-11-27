#ifndef GPIO_H_INCLUDED
	#define GPIO_H_INCLUDED
	
	static GPIO_InitTypeDef GPIO_Struct;
	
	//==============================================================================
	// GPIO pin control functions
	//==============================================================================
	#define pin_set(gpio, pin, state)	if(state) gpio->ODR |= (pin); else gpio->ODR &= ~(pin)
	#define pin_on(gpio, pin)			gpio->ODR |= (pin)
	#define pin_off(gpio, pin)			gpio->ODR &= ~(pin)
	
	
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
	void GPIO_Set_Output(GPIO_TypeDef *GPIO,uint16_t Pin)
	{
		GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_Struct.Pull = GPIO_NOPULL;
		GPIO_Struct.Speed = GPIO_SPEED_HIGH;
		GPIO_Struct.Pin = Pin;
		HAL_GPIO_Init(GPIO, &GPIO_Struct);
	}
	
	// set a pin to an input
	void GPIO_Set_Input(GPIO_TypeDef *GPIO, uint16_t Pin)
	{
		GPIO_Struct.Mode = GPIO_MODE_INPUT;
		GPIO_Struct.Pull = GPIO_NOPULL;
		GPIO_Struct.Speed = GPIO_SPEED_HIGH;
		GPIO_Struct.Pin = Pin;
		HAL_GPIO_Init(GPIO, &GPIO_Struct);
	}
	
	// init all the pins the STM32 will use.
	void GPIO_Init()
	{
		// enable the clocks for the ports we want to use
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		
		// enable solenoid output shift register pins
		GPIO_Set_Output(SOL_SR_GPIO,SOL_SR_DATA);
		GPIO_Set_Output(SOL_SR_GPIO,SOL_SR_CLOCK);
		GPIO_Set_Output(SOL_SR_GPIO,SOL_SR_LATCH);
		
		// enable debug pins
		GPIO_Set_Output(DEBUG_GPIO,DEBUG_0);
		GPIO_Set_Output(DEBUG_GPIO,DEBUG_1);
		GPIO_Set_Output(DEBUG_GPIO,DEBUG_WARNING_LED);
		GPIO_Set_Output(DEBUG_GPIO,DEBUG_ERROR_LED);
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
	
#endif // GPIO_H_INCLUDED
