/**
 ******************************************************************************
 * @file	 GPIO/GPIO_IOToggle/Src/main.c
 * @author	MCD Application Team
 * @version V1.0.0
 * @date	 06-May-2016
 * @brief	This example describes how to configure and use GPIOs through
 *			 the STM32F4xx HAL API.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *	1. Redistributions of source code must retain the above copyright notice,
 *		this list of conditions and the following disclaimer.
 *	2. Redistributions in binary form must reproduce the above copyright notice,
 *		this list of conditions and the following disclaimer in the documentation
 *		and/or other materials provided with the distribution.
 *	3. Neither the name of STMicroelectronics nor the names of its contributors
 *		may be used to endorse or promote products derived from this software
 *		without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo_144.h"
#include "../../../../app/key.h"
#include <stdlib.h>

/** @addtogroup STM32F4xx_HAL_Examples
 * @{
 */

/** @addtogroup GPIO_IOToggle
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static GPIO_InitTypeDef GPIO_Struct;


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);



#define pin_set(gpio, pin, state)	if(state) gpio->ODR |= (pin); else gpio->ODR &= ~(pin)
#define pin_on(gpio, pin)			gpio->ODR |= (pin)
#define pin_off(gpio, pin)			gpio->ODR &= ~(pin)

//==============================================================================
// keys
//==============================================================================
#define KEYS ((uint16_t)37)				// 37 total keys on the xylophone.
										// halfsteps from [middle C (261.6 Hz)] thru [double-high C (2093.0 Hz)]
#define MIDI_OFFSET ((uint16_t)60)		// the xylophone starts on middle C, and goes up from there.


//==============================================================================
// clock system
//==============================================================================
// main CPU frequency
#define CPU_FREQ ((uint32_t)100000000)			// 100 MHz (10 ns timebase)

//==============================================================================
// solenoid system
//==============================================================================
// timer used for solenoid timebase and control
#define SOL_TIM			TIM2							// the timer used for the solenoid system.
														// if you change this, you need to change solenoid_init(), and look at TIM2_IRQHandler() to update them to the new timer you are using.
#define SOL_TIM_FREQ	((uint32_t)1000000)				// 1 MHz (1 us timebase) for controlling solenoids
#define SOL_TIM_PSC		(CPU_FREQ/SOL_TIM_FREQ - 1)		// the prescaler setting for the solenoid timer
#define SOL_TIM_ARR 0xFFFFFFFF							// this is where the solenoid timer rolls over (automatic reload register). At 1-us ticks, this is 1.2 hours. The timer will NEVER reach this, because the timer will automatically get reset to 0 when there are no notes to play
#define SOL_TIM_OFF 0xFFFFFFFF							// this is a flag that tells us that the solenoid is off.
#define SOL_TIME_TOO_LONG (SOL_TIM_FREQ/100)			// maximum time a solenoid should be on is 10  ms. Anything longer  than this indicates an error in the program because 10  ms is fucking ridiculously long.
#define SOL_TIME_TOO_SHORT (SOL_TIM_FREQ/2000)			// minimum time a solenoid should be on is 0.5 ms. Anything shorter than this indicates an error in the program because 0.5 ms is fucking ridiculously short.
volatile uint32_t solenoid_timing_array[KEYS];					// records when you need to shut off each solenoids.
volatile uint8_t solenoid_states[KEYS];							// records the state of each solenoids. 0 = off, 1 means on.
volatile uint8_t solenoid_all_are_off;							// if all the solenoids are off, this is a 1. otherwise, it is a 0
// solenoid control shift register
#define SOL_SR_GPIO GPIOC			// the register that is used to shift out data to control the shift registers
#define SOL_SR_DATA	GPIO_PIN_1		// SER (74hc595 pin 14)	data in
#define SOL_SR_LATCH GPIO_PIN_4		// RCK (74hc595 pin 12) register clock (update output)
#define SOL_SR_CLOCK GPIO_PIN_5		// SCK (74hc595 pin 11) data clock in
#define SOL_SR_DIR 1				// direction that the solenoid shift register shifts out key data

#if(SOL_TIM_OFF < SOL_TIM_MOD)
#error "You cannot make your solenoid OFF flag be a value less than the solenoid modulo value."
#endif




// shift register input pins
// port F pins
#define SRI_DATA  GPIO_PIN_2
#define SRI_LATCH GPIO_PIN_1
#define SRI_CLOCK GPIO_PIN_0

// input pins
// port C pins
#define IN_1 GPIO_PIN_8
#define IN_2 GPIO_PIN_9
#define IN_3 GPIO_PIN_10
#define IN_4 GPIO_PIN_11
#define IN_5 GPIO_PIN_12
#define IN_6 GPIO_PIN_2
#define IN_7 GPIO_PIN_2
#define IN_8 GPIO_PIN_3

//GPIO_PinState input_state[INPUT_PINS];
//GPIO_TypeDef input_pin_port[INPUT_PINS];

#define input_pin_port_1 GPIOC
#define input_pin_port_2 GPIOC
#define input_pin_port_3 GPIOC
#define input_pin_port_4 GPIOC
#define input_pin_port_5 GPIOC
#define input_pin_port_6 GPIOD
#define input_pin_port_7 GPIOG
#define input_pin_port_8 GPIOG

#define KEYS 8
#define KEY_BYTE_SIZE 1
#define KEY_COOLDOWN 10

uint8_t keyInput[KEY_BYTE_SIZE];
int8_t keyInputCoolDown[KEYS];
uint8_t keyOutput[KEY_BYTE_SIZE];

#define SONG_LENGTH 1000
int currentTime = 0;
int totalNotes = 0;

#define size 14
int i, j;
int song[size] = {
		  0,1,2,3,4,5,6,7,6,5,4,3,2,1
};


// this function happens when something FATAL happens. The program cannot continue, and it will freeze here indefinitely.
// this can be used as a breakpoint when you are debugging to see WHY the error happened.
void error(char *message)
{
	// TODO: turn on the ERROR light
	while(1)
	{
		; // freeze program
	}
}


// this function is called when something bad happens, but it isn't the end of the world, and the xylophone will keep on playing.
// it is intended to be used as a breakpoint when you are debugging WHY the warning happened.
void warning(char *message)
{
	// TODO: turn on the WARNING light
}


//=============================================================================
// clock data out of the STM32 into a shift register (serial-in parallel-out shift register such as 74hc595)
//
// GPIOx  		the port that must be used for all three shift register pins
// clockPin 	the pin that clocks data into the shift register
// dataPin		the data pin that the shift register will be inputting.
// latchPin		the pin that updates the register's output pins when all the data has been shifted into it.
// bits			the number of bits you want to shift into the register.
// *data		a pointer to the array (of uint8_t type). The array is treated like a bool (0 or 1). those values are shifted out.
// dir			the direction of data to be shifted out.
//				dir=0  =>  data[0],      data[1],      data[2],      ...
//				else   =>  data[bits-1], data[bits-2], data[bits-3], ...
//=============================================================================
void shift_out(GPIO_TypeDef* GPIO, uint8_t clockPin, uint8_t dataPin, uint8_t latchPin, uint32_t bits, uint8_t *data, uint8_t dir)
{
	uint32_t b;
	uint32_t i;
	for(i=0; i < bits; i++)
	{
		if(dir) {b = (bits-1) - i;} else {b = i;}
		// output the right data
		pin_set(GPIO,dataPin,data[b]);
		// clock data in
		pin_off(GPIO,clockPin);
		pin_on(GPIO,clockPin);
		pin_off(GPIO,clockPin);
	}
	// update the shift register
	pin_off(GPIO,latchPin);
	pin_on(GPIO,latchPin);
	pin_off(GPIO,latchPin);
}


void clock_in(GPIO_TypeDef* GPIOx, uint16_t clockPin, uint16_t dataPin, uint16_t latchPin, uint8_t outputSize, uint8_t *data) {
	uint8_t i, j;
	uint8_t inData = 0;

	// cycle the latch to clock all the bits in
	GPIOx->BSRR = (uint32_t)latchPin << 16U;
	GPIOx->BSRR = latchPin;

	for(i = 0; i < outputSize; i++) {
		for(j = 0; j < 8; j++) {
			// read data from pin
			inData = HAL_GPIO_ReadPin(GPIOx, dataPin);

			// clock data in to get next bit
			GPIOx->BSRR = (uint32_t)clockPin << 16U;
			GPIOx->BSRR = clockPin;

			// insert bits into the output data
			if(inData != 0)
				data[i] |= (1 << j);
		}
	}
}


//==============================================================================
// initialize all solenoid-related things
//==============================================================================
void solenoid_init()
{
	//--------------------------------------------------------------------------
	// set initial states of the solenoid shift-register pins
	//--------------------------------------------------------------------------
	pin_off(SOL_SR_GPIO,SOL_SR_CLOCK);
	pin_off(SOL_SR_GPIO,SOL_SR_DATA);
	pin_off(SOL_SR_GPIO,SOL_SR_LATCH);
	
	//--------------------------------------------------------------------------
	// setup solenoid variables
	//--------------------------------------------------------------------------
	// turn off all of the solenoids
	uint16_t i;
	for(i=0; i<KEYS; i++)
	{
		solenoid_timing_array[i] = SOL_TIM_OFF;
		solenoid_states[i] = 0;
	}
	solenoid_all_are_off = 1;
	
	//--------------------------------------------------------------------------
	// configure timer 2 
	//--------------------------------------------------------------------------
	// enable the solenoid timer (start counting)
	__HAL_RCC_TIM2_CLK_ENABLE();
	// configure solenoid timer
	SOL_TIM->PSC = SOL_TIM_PSC;	 	// Set prescaler. Division factor = (PSC + 1)
	SOL_TIM->ARR = SOL_TIM_ARR;		// Auto reload value. period = (ARR + 1)
	//SOL_TIM->DIER = TIM_DIER_UIE;	// Enable update interrupt (timer level)
	SOL_TIM->DIER = TIM_DIER_CC1IE;	// Enable counter compare interrupt.
	// SOL_TIM->CCR1 = [value to create interrupt at]; 
	SOL_TIM->CR1 = TIM_CR1_CEN;		// Enable timer
	// enable solenoid timer interrupt handler
	NVIC_EnableIRQ(TIM2_IRQn);
	SOL_TIM->CCR1 = SOL_TIM_OFF;		// init the compare register to a value it will never reach.
	SOL_TIM->CNT = 0;				// init the counter at 0.
	SOL_TIM->EGR |= TIM_EGR_UG;		// generate a timer update (this updates all the timer settings that were just configured). See RM0402.pdf section 17.4.6  "TIMx event generation register (TIMx_EGR)"
}


//=============================================================================
// figure out how long to wait before turning off the next solenoid.
//=============================================================================
void solenoid_interrupt_recalculate()
{
	// TODO: write this function
}


//=============================================================================
// timer 2 Interrupt Handler
//=============================================================================
void TIM2_IRQHandler(void)
{
	/*
	if (TIM2->SR & TIM_SR_UIF) // if UIF flag is set
	{
		TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag
	}
	*/
	if(TIM2->SR & TIM_SR_CC1IF) // if the counter compare flag is set,
	{
		TIM2->SR &= ~TIM_SR_CC1IF;	// clear it.
		SOL_SR_GPIO->ODR &= ~SOL_SR_CLOCK; // clear pin state to show that it was done
		// TODO: implement determining which keys to turn off.
		solenoid_interrupt_recalculate();
	}
}


//=============================================================================
// play a solenoid
// key		the key you want to play
// length	the amount of time the solenoid will be on for in units of microseconds.
//=============================================================================
void solenoid_play(uint8_t key, uint32_t length)
{
	if(key >= KEYS)
	{
		warning("you tried to play a key out of the valid range! I'm going to play the highest key to let you know you suck and your program is broken.");
		key = KEYS-1;
	}
	if(length >= SOL_TIME_TOO_LONG)
	{
		warning("you are trying to play a key for too long. I'll still play the key, but there is probably an error in the code that is making it so long...");
		length = SOL_TIME_TOO_LONG;
	}
	if(length <= SOL_TIME_TOO_SHORT)
	{
		warning("you are trying to play a key for too short. I'll still play the key, but there is probably an error in the code that is making it so short...");
		length = SOL_TIME_TOO_SHORT;
	}
	
	// record that you are turning on this solenoid.
	solenoid_states[key] = 1;
	// turn on the solenoid.
	shift_out(SOL_SR_GPIO,SOL_SR_CLOCK,SOL_SR_DATA,SOL_SR_LATCH,KEYS,solenoid_states,SOL_SR_DIR);
	// record when we need to turn off this key.
	solenoid_timing_array[key] = SOL_TIM->CNT + length;
	// at least one solenoid is now on, so make this 0.
	solenoid_all_are_off = 0;
	// figure out when the next interrupt will have to be to shut off the solenoid at the right time. 
	solenoid_interrupt_recalculate();
}




int main(void)
{
    // Initialize the hardware access library
	HAL_Init();
	// Configure the system clock to 100 MHz
	SystemClock_Config();
	
  // enable port c
  __HAL_RCC_GPIOC_CLK_ENABLE();
  // enable port f
  __HAL_RCC_GPIOF_CLK_ENABLE();
  // enable port d
  __HAL_RCC_GPIOD_CLK_ENABLE();
  //enable port g
  __HAL_RCC_GPIOG_CLK_ENABLE();
  
	// setup struct
	GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Struct.Pull = GPIO_NOPULL;
	GPIO_Struct.Speed = GPIO_SPEED_HIGH;
	
	// enable solenoid output shift register pins
	GPIO_Struct.Pin = SOL_SR_DATA;
	HAL_GPIO_Init(GPIOC, &GPIO_Struct);
	GPIO_Struct.Pin = SOL_SR_LATCH;
	HAL_GPIO_Init(GPIOC, &GPIO_Struct);
	GPIO_Struct.Pin = SOL_SR_CLOCK;
	HAL_GPIO_Init(GPIOC, &GPIO_Struct);
	
     // enable input shift register
  GPIO_Struct.Pin = SRI_LATCH;
  HAL_GPIO_Init(GPIOF, &GPIO_Struct);
  GPIO_Struct.Pin = SRI_CLOCK;
  HAL_GPIO_Init(GPIOF, &GPIO_Struct);
  GPIO_Struct.Mode = GPIO_MODE_INPUT;
  GPIO_Struct.Pin = SRI_DATA;
  HAL_GPIO_Init(GPIOF, &GPIO_Struct);
  // set latch pin to high
  GPIOF->BSRR = SRI_LATCH;
    
	// test led
	GPIO_Struct.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOC, &GPIO_Struct);
	
 /*
 // JP's code for the song
    uint32_t i;
  // current song to be played
  Note *currentSong = init_note(KEY_TRACK_EMPTY, 0, 100);
  Note *noteToPlay = NULL;
  int previousNotesPlayed[KEYS];

  // reset key output
  for(i = 0; i < KEY_BYTE_SIZE; i++)
	  keyOutput[i] = 0;

  // set key cool down
    for(i = 0; i < KEY_BYTE_SIZE; i++)
  	  keyInputCoolDown[i] = -1;

  // clear the LEDs
  clock_out(GPIOC, SOL_SR_CLOCK, SOL_SR_DATA, SOL_SR_LATCH, 8, keyOutput);

  // main loop
  while (1)
  {
	  // reset key input
	  for(i = 0; i < KEY_BYTE_SIZE; i++)
		  keyInput[i] = 0;

	  // get all of the key inputs
	  clock_in(GPIOF, SRI_CLOCK, SRI_DATA, SRI_LATCH, KEY_BYTE_SIZE, keyInput);

	  uint8_t curKey = 0;
	  for(i = 0; i < KEY_BYTE_SIZE; i++) {
		  uint8_t mask = 1;

		  for(j = 0; j < 8; j++) {
			  if((mask & keyInput[i]) != 0 && keyInputCoolDown[curKey] == -1) {
				  Note* n = init_note(curKey, currentTime, 10);
				  insert_note(&currentSong, n);
				  totalNotes++;
				  keyInputCoolDown[curKey] = (currentTime + KEY_COOLDOWN) % SONG_LENGTH;
			  }
			  // get next bit
			  mask <<= 1;
			  // increment current key
			  curKey++;
		  }
	  }

	  if(currentSong->key != KEY_TRACK_EMPTY) {
		  if(noteToPlay == NULL) {
			  noteToPlay = currentSong;
		  } else if(noteToPlay->time == currentTime) {
			  setKeyOutput(noteToPlay->key, keyOutput);
			  previousNotesPlayed[noteToPlay->key] = (currentTime + noteToPlay->intensity) % SONG_LENGTH;
			  noteToPlay = noteToPlay->next;
		  }
	  }

	  for(i = 0; i < KEYS; i++) {
		  if(previousNotesPlayed[i] == currentTime) {
			  removeKeyOutput(i, keyOutput);
			  previousNotesPlayed[i] = -1;
		  }
	  }

	  // increment the time
	  currentTime++;
	  if(currentTime > SONG_LENGTH)
		  currentTime = 0;

	  // reset key cool down if it is time too
	  for(i = 0; i < KEYS; i++) {
		  if(keyInputCoolDown[i] == currentTime)
			  keyInputCoolDown[i] = -1;
	  }

	  clock_out(GPIOC, SOL_SR_CLOCK, SOL_SR_DATA, SOL_SR_LATCH, KEY_BYTE_SIZE, keyOutput);

	  // delay
	  HAL_Delay(1);
    */

	// initialize solenoid stuff.
	solenoid_init();
	SOL_TIM->CNT = 0;
	uint8_t i = 0;
	uint8_t state = 1;
	// uint16_t T = 512;
	// main loop
	while(1)
	{
		
//		SOL_SR_GPIO->ODR |= SOL_SR_CLOCK; // turn on SOL_SR_CLOCK pin
//		SOL_TIM->CCR1 = SOL_TIM->CNT + 10000;
//		HAL_Delay(8);
		if(i>=KEYS) { i = 0; state = !state; }
		solenoid_states[i] = state;
		shift_out(SOL_SR_GPIO,SOL_SR_CLOCK,SOL_SR_DATA,SOL_SR_LATCH,KEYS,(uint32_t *)solenoid_states,SOL_SR_DIR);
		HAL_Delay(100);
		i++;
	}
}

/**
 * @brief	System Clock Configuration
 *			The system Clock is configured as follow : 
 *				System Clock source				= PLL (HSE)
 *				SYSCLK(Hz)							= 100000000
 *				HCLK(Hz)								= 100000000
 *				AHB Prescaler						= 1
 *				APB1 Prescaler						= 2
 *				APB2 Prescaler						= 1
 *				HSE Frequency(Hz)					= 8000000
 *				PLL_M									= 8
 *				PLL_N									= 200
 *				PLL_P									= 2
 *				PLL_Q									= 7
 *				PLL_R									= 2
 *				VDD(V)								 = 3.3
 *				Main regulator output voltage	= Scale1 mode
 *				Flash Latency(WS)					= 3
 * @param	None
 * @retval None
 */
static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;
	
	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE()
	;
	
	/* The voltage scaling allows optimizing the power consumption when the device is 
	 clocked below the maximum system frequency, to update the voltage scaling value 
	 regarding system frequency refer to product datasheet.	*/
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 200;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	RCC_OscInitStruct.PLL.PLLR = 2;
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	
	if (ret != HAL_OK)
	{
		error("Could not HAL_RCC_OscConfig()");
	}
	
	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
	if (ret != HAL_OK)
	{
		error("Could not HAL_RCC_ClockConfig()");
	}
}

#ifdef	USE_FULL_ASSERT

/**
 * @brief	Reports the name of the source file and the source line number
 *			where the assert_param error has occurred.
 * @param	file: pointer to the source file name
 * @param	line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{	
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	error("assert failed");
}
#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
