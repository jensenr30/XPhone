#include "GPIO.h"
static GPIO_InitTypeDef GPIO_Struct;

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
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	
	// enable solenoid output shift register pins
	GPIO_set_output(SOL_SR_DATA_GPIO,SOL_SR_DATA);
	GPIO_set_output(SOL_SR_CLOCK_GPIO,SOL_SR_CLOCK);
	GPIO_set_output(SOL_SR_LATCH_GPIO,SOL_SR_LATCH);
	
	// enable key input shift register pins
	GPIO_set_input(KEY_INPUT_GPIO,KEY_INPUT_DATA);
	GPIO_set_output(KEY_INPUT_GPIO,KEY_INPUT_CLOCK);
	GPIO_set_output(KEY_INPUT_GPIO,KEY_INPUT_LATCH);
	
	// enable debug pins
	GPIO_set_output(DEBUG_WARNING_LED_GPIO,DEBUG_WARNING_LED);
	GPIO_set_output(DEBUG_ERROR_LED_GPIO,DEBUG_ERROR_LED);
	GPIO_set_output(DEBUG_0_GPIO,DEBUG_0);
	GPIO_set_output(DEBUG_LED_GPIO,DEBUG_LED);
	
	// initialize debug pins
	pin_off(DEBUG_WARNING_LED_GPIO, DEBUG_WARNING_LED);
	pin_off(DEBUG_ERROR_LED_GPIO, DEBUG_ERROR_LED);
	pin_off(DEBUG_0_GPIO,DEBUG_0);
	pin_off(DEBUG_LED_GPIO,DEBUG_LED);
	
	
	// control panel pins
	GPIO_set_input(CTRL_IN_ARM_GPIO,CTRL_IN_ARM);
	GPIO_set_input(CTRL_IN_CAL_GPIO,CTRL_IN_CAL);
	GPIO_set_input(CTRL_IN_CLEAR_GPIO,CTRL_IN_CLEAR);
	GPIO_set_input(CTRL_IN_TIMING_GPIO,CTRL_IN_TIMING);
	GPIO_set_input(CTRL_IN_SYNC_GPIO,CTRL_IN_SYNC);
	
	// control panel outputs
	GPIO_set_output(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);
	GPIO_set_output(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);
	GPIO_set_output(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B);
	
	//initialize control pins
	pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);
	pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);
	pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B);
	
	// initialize the amux shift register pins
	GPIO_set_output(AMUX_SR_DATA_GPIO,AMUX_SR_DATA);
	GPIO_set_output(AMUX_SR_CLOCK_GPIO,AMUX_SR_CLOCK);
	// initialize states of amux pins
	pin_off(AMUX_SR_DATA_GPIO,AMUX_SR_DATA);
	pin_off(AMUX_SR_CLOCK_GPIO,AMUX_SR_CLOCK);
	
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
void shift_out(GPIO_TypeDef* clockGPIO, GPIO_Pin_Type clockPin, GPIO_TypeDef* dataGPIO, GPIO_Pin_Type dataPin, GPIO_TypeDef* latchGPIO, GPIO_Pin_Type latchPin, uint32_t bits, uint8_t *data, uint8_t dir)
{
	uint32_t i;								// used for incrementing through all bits 
	uint32_t b;								// used for indexing the right element of the array (allows us to do MSB first or LSB first)
	for (i = 0; i < bits; i++)
	{
		if (dir)								// if dir is true,
			b = (bits - 1) - i;						// MSB (most significant bit first)
		else									// if dir is false,
			b = i;									// LSB (least significant bit first)
		pin_set(dataGPIO, dataPin, data[b]);		// output the right data
		pin_off(clockGPIO, clockPin);				// clock (advance) shift register
		pin_on(clockGPIO, clockPin);					// ^
	}
	pin_off(latchGPIO, latchPin);				// update the shift register
	pin_on(latchGPIO, latchPin);					// ^
	pin_off(latchGPIO, latchPin);				// ^
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

