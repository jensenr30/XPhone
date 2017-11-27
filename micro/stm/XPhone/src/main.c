/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo_144.h"
#include <stdlib.h>
#include "main.h"
#include "GPIO.h"
#include "key.h"

//==============================================================================
// clock system
//==============================================================================
// main CPU frequency
#define CPU_FREQ ((uint32_t)100000000)			// 100 MHz (10 ns timebase)

static void SystemClock_Config(void);

//=============================================================================
// this function happens when something FATAL happens. The program cannot continue, and it will freeze here indefinitely.
// this can be used as a breakpoint when you are debugging to see WHY the error happened.
//=============================================================================
void error(char *message)
{
	pin_on(DEBUG_GPIO, DEBUG_ERROR_LED);
	while (1) {;} // freeze program
}

//=============================================================================
// this function is called when something bad happens, but it isn't the end of the world, and the xylophone will keep on playing.
// it is intended to be used as a breakpoint when you are debugging WHY the warning happened.
//=============================================================================
void warning(char *message)
{
	pin_on(DEBUG_GPIO, DEBUG_WARNING_LED);
	return;
}


//==============================================================================
// solenoid system
//==============================================================================
// timer used for solenoid timebase and control
#define SOL_TIM			TIM2							// the timer used for the solenoid system.
														// if you change this, you need to change solenoid_init(), and look at TIM2_IRQHandler() to update them to the new timer you are using.
#define SolTimType uint32_t								// this is the data type for solenoid timing
#define SOL_TIM_FREQ	((SolTimType)1000000)			// 1 MHz (1 us timebase) for controlling solenoids
#define SOL_TIM_PSC		(CPU_FREQ/SOL_TIM_FREQ - 1)		// the prescaler setting for the solenoid timer
#define SOL_TIM_ARR (0xFFFFFFFF)						// this is where the solenoid timer rolls over (automatic reload register). At 1-us ticks, this is 1.2 hours. The timer will NEVER reach this, because the timer will automatically get reset to 0 when there are no notes to play
#define SOL_TIM_OFF (0xFFFFFFFF)						// this is a flag that tells us that the solenoid is off.
#define SOL_TIME_TOO_LONG (SOL_TIM_FREQ/100)			// [us] maximum time a solenoid should be on is 10  ms. Anything longer  than this indicates an error in the program because 10	ms is fucking ridiculously long.
#define SOL_TIME_TOO_SHORT (SOL_TIM_FREQ/2000)			// [us] minimum time a solenoid should be on is 0.5 ms. Anything shorter than this indicates an error in the program because 0.5 ms is fucking ridiculously short.
#define SOL_TIM_TURN_OFF_NOMINAL ((SolTimType)50)		// [us] the typical time it takes to run through the solenoid timer interrupt handling routine. 
#define SOL_TIM_TURN_OFF_WINDOW ((SolTimType)70)		// [us] if two keys end within this period of time, they will be shut off simultaneously. This is in place because the interrupt handling functions cannot run instantaneously. They take a few 10's of microseconds to run, and we need to accommodate that.

volatile SolTimType solenoid_timing_array[KEYS];		// records when you need to shut off each solenoids.
volatile KeyStateType solenoid_states[KEYS];			// records the state of each solenoids. 0 = off, 1 means on. After modifying this array, you must run solenoid_update() for the solenoids to actually be turned on/off.

// this function will update the sates of the solenoid drivers.
#define solenoid_update() shift_out(SOL_SR_GPIO,SOL_SR_CLOCK,SOL_SR_DATA,SOL_SR_LATCH,KEYS,(KeyStateType *)solenoid_states,SOL_SR_DIR)

#if(SOL_TIM_OFF < SOL_TIM_MOD)
#error "You cannot make your solenoid OFF flag be a value less than the solenoid modulo value."
#endif

// JP's key input/output definitions
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

//==============================================================================
// initialize all solenoid-related things
//==============================================================================
void solenoid_init()
{
	//--------------------------------------------------------------------------
	// set initial states of the solenoid shift-register pins
	//--------------------------------------------------------------------------
	pin_off(SOL_SR_GPIO, SOL_SR_CLOCK);
	pin_off(SOL_SR_GPIO, SOL_SR_DATA);
	pin_off(SOL_SR_GPIO, SOL_SR_LATCH);
	
	//--------------------------------------------------------------------------
	// setup solenoid variables
	//--------------------------------------------------------------------------
	// turn off all of the solenoids
	KeyType i;
	for (i = 0; i < KEYS; i++)
	{
		solenoid_timing_array[i] = SOL_TIM_OFF;
		solenoid_states[i] = 0;
	}
	
	//--------------------------------------------------------------------------
	// configure timer 2 
	//--------------------------------------------------------------------------
	// enable the solenoid timer (start counting)
	__HAL_RCC_TIM2_CLK_ENABLE();
	SOL_TIM->PSC = SOL_TIM_PSC;	 				// Set prescaler. Division factor = (PSC + 1)
	SOL_TIM->ARR = SOL_TIM_ARR;					// Auto reload value. period = (ARR + 1)
	//SOL_TIM->DIER = TIM_DIER_UIE;				// Enable update interrupt (timer level)
	SOL_TIM->DIER = TIM_DIER_CC1IE;				// Enable counter compare interrupt.
	SOL_TIM->CCR1 = SOL_TIM_OFF;				// initialize the counter compare interrupt to happen when the timer reaches the max. 
	SOL_TIM->CR1 = TIM_CR1_CEN;					// Enable timer
	NVIC_EnableIRQ(TIM2_IRQn);					// enable solenoid timer interrupt handler
	SOL_TIM->CCR1 = SOL_TIM_OFF;				// init the compare register to a value it will never reach.
	SOL_TIM->CNT = 0;							// init the counter at 0.
	SOL_TIM->EGR |= TIM_EGR_UG;					// generate a timer update (this updates all the timer settings that were just configured). See RM0402.pdf section 17.4.6	"TIMx event generation register (TIMx_EGR)"
}


//=============================================================================
// figure out how long to wait before turning off the next solenoid.
// benchmark: 9 us with 37 keys.
//=============================================================================
void solenoid_interrupt_recalculate()
{
	SolTimType min_time = SOL_TIM_OFF;			// keep track of the soonest time that any of the solenoids need to be turned off. Start at the maximum unsigned 32-bit value (SOL_TIM_OFF). 
	KeyType k;									// an index to go through all the keys
	//-------------------------------------------------------------------------
	// find the soonest time that the next key will need to be turned off.
	//-------------------------------------------------------------------------
	for(k=0; k<KEYS; k++)						// for each key...
	{
		if(solenoid_timing_array[k] < min_time)		// if [the time at which this keys must be turned off] is sooner than [all the other keys],
			min_time = solenoid_timing_array[k];		// record this as the soonest one
	}
	//-------------------------------------------------------------------------
	// do a check to make sure you didn't miss something
	//-------------------------------------------------------------------------
	if(min_time < SOL_TIM->CNT)						// make a warning if you missed a key
	{
		warning("You missed a key someone! Your minimum time is less than your solenoid timer count value, so you must have missed something...");
	}
	else											// otherwise,
	{
		SOL_TIM->CCR1 = min_time;						// set the next interrupt time.
	}
}


//=============================================================================
// This will shut off all the right solenoids. If they are all shut off, it
// will reset the solenoid timer counter as well to avoid overflow issue.
// benchmark time = 15.2 us for 37 keys.
//=============================================================================
void solenoid_shut_off_the_right_ones()
{
	SolTimType T_off_min = SOL_TIM->CCR1;			// grab the time that we are supposed to shut off
	SolTimType T_off_max = T_off_min + SOL_TIM_TURN_OFF_WINDOW;	// this is the maximum time that we can turn off.
	KeyType k;										// index into key arrays
	KeyStateType keys_all_off = 1;					// this keeps track of whether or not all the keys are off. We start assuming they are, and try to disprove it in the for() loop.
	
	for (k = 0; k < KEYS; k++)						// go thru all the keys...
	{
		if (	(solenoid_timing_array[k] >= T_off_min) && 		// if this solenoid is supposed to be shut off
				(solenoid_timing_array[k] <= T_off_max)	)		//  in this particular window of time,
		{
			solenoid_states[k] = 0;							// put that information in the states array. You will need to run solenoid_update() to actually change the states of the solenoids.
			solenoid_timing_array[k] = SOL_TIM_OFF;			// flag this key as being off in the timing array.
		}
		if (solenoid_states[k])	keys_all_off = 0;			// if any key is currently on, then [all keys off] is false.
	}
	if(keys_all_off) SOL_TIM->CNT = 0;				// if all the keys are off now, it is a goot time to reset the timer to 0. This is done to avoid the overflow problem. The timer will always be reset long before we run out of time.
}


//=============================================================================
// timer 2 Interrupt Handler
// benchmark time = 43.6 us with 37 keys.
//=============================================================================
void TIM2_IRQHandler(void)
{
	pin_on(DEBUG_GPIO,DEBUG_0);						// turn on debug_0 pin to allow someone to see when the TIM2_IRQHandler starts
//	// this is for catching the interrupt that is generated when the value gets updated (rollover, auto-reload, modify, etc.)
//	// this is unnecessary for the time being because I only need to interrupt on the CCR1 register (below)
//	if (TIM2->SR & TIM_SR_UIF) // if UIF flag is set
//	{
//	TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag
//	}
	
	if (SOL_TIM->SR & TIM_SR_CC1IF) 				// if the counter compare flag is set,
	{
		SOL_TIM->SR &= ~TIM_SR_CC1IF;					// clear it.
		//pin_on(DEBUG_GPIO,DEBUG_1);
		solenoid_shut_off_the_right_ones();				// shut off the right solenoids.
		//pin_off(DEBUG_GPIO,DEBUG_1);
		solenoid_interrupt_recalculate();				// calculate when the next interrupt should be.
		solenoid_update();								// update the states of the solenoids
	}
	pin_off(DEBUG_GPIO,DEBUG_0);					// turn off the debug_1 pin. This allows someone to see how long it took to run through the TIM2_IRQHandler() function.
}


//=============================================================================
// play a solenoid
// key		the key you want to play
// length	the amount of time the solenoid will be on for in units of microseconds.
//=============================================================================
void solenoid_play(KeyType key, SolTimType length)
{
	if (key >= KEYS)
	{
		warning("you tried to play a key out of the valid range! I'm going to play the highest key to let you know you suck and your program is broken.");
		key = KEYS - 1;
	}
//	// TODO: uncomment this
//	if (length >= SOL_TIME_TOO_LONG)
//	{
//		warning("you are trying to play a key for too long. I'll still play the key, but there is probably an error in the code that is making it so long...");
//		length = SOL_TIME_TOO_LONG;
//	}
	if (length <= SOL_TIME_TOO_SHORT)
	{
		warning("you are trying to play a key for too short. I'll still play the key (at the minimum length), but there is probably an error in the code (or calibration data) that is making it so short...");
		length = SOL_TIME_TOO_SHORT;
	}
	
	// record that you are turning on this solenoid.
	solenoid_states[key] = 1;
	// update the states of the solenoids
	solenoid_update();
	// record when we need to turn off this key. Compensate for the time it will take to execute the interrupt handling routine.
	solenoid_timing_array[key] = SOL_TIM->CNT + length - SOL_TIM_TURN_OFF_NOMINAL;
	// figure out when the next interrupt will have to be to shut off the solenoid at the right time. 
	solenoid_interrupt_recalculate();
}


//=============================================================================
// where the program starts
//=============================================================================
int main(void)
{
	
	// TODO: write a watchdog type of code that ensures that the solenoid timer doesn't go too high.
	// you would need to gracefully return the solenoid timer to 0 while adjusting the off-times of all of the currently turned-on solenoids.
	
	//-------------------------------------------------------------------------
	// Initialize
	//-------------------------------------------------------------------------
	HAL_Init();							// Initialize the hardware access library
	SystemClock_Config();				// Configure the system clock to 100 MHz
	GPIO_Init();						// set up all GPIO pins for everything.
	solenoid_init();					// initialize all solenoid stuff.
	
	//-------------------------------------------------------------------------
	// Main Program Loop
	//-------------------------------------------------------------------------
	// Jensen's little loop for testing the solenoid_play() function.
	KeyStateType i = 0;					// index
	SolTimType ms2us = 1000;			// milliseconds to microseconds conversion
	while (1)							// main program loop
	{
		if(i >= KEYS) i = 0;			// reset the index if it gets greater than the number of keys we have.
		solenoid_play(i++,4*ms2us);	// play a key, and increment the index
		HAL_Delay(1);					// wait a bit
	}
	
//	 // JP's code for the song
//	uint32_t i;
//	// current song to be played
//	Note *currentSong = init_note(KEY_TRACK_EMPTY, 0, 100);
//	Note *noteToPlay = NULL;
//	int previousNotesPlayed[KEYS];
//	
//	// reset key output
//	for(i = 0; i < KEY_BYTE_SIZE; i++)
//		keyOutput[i] = 0;
//	
//	// set key cool down
//	for(i = 0; i < KEY_BYTE_SIZE; i++)
//		keyInputCoolDown[i] = -1;
//	
//	// main loop
//	while (1)
//	{
//		// TODO: why the fuck is this here? clock_in() should overwrite the array...
//		// reset key input
//		for(i = 0; i < KEY_BYTE_SIZE; i++)
//			keyInput[i] = 0;
//		
//		// get all of the key inputs
//		clock_in(KEY_INPUT_GPIO, KEY_INPUT_CLOCK, KEY_INPUT_DATA, KEY_INPUT_LATCH, KEY_BYTE_SIZE, keyInput);
//		
//		uint8_t curKey = 0;
//		for(i = 0; i < KEY_BYTE_SIZE; i++) {
//			uint8_t mask = 1;
//			
//			for(j = 0; j < 8; j++) {
//				if((mask & keyInput[i]) != 0 && keyInputCoolDown[curKey] == -1) {
//					Note* n = init_note(curKey, currentTime, 10);
//					insert_note(&currentSong, n);
//					totalNotes++;
//					keyInputCoolDown[curKey] = (currentTime + KEY_COOLDOWN) % SONG_LENGTH;
//				}
//				// get next bit
//				mask <<= 1;
//				// increment current key
//				curKey++;
//			}
//		}
//		
//		if(currentSong->key != KEY_TRACK_EMPTY) {
//			if(noteToPlay == NULL) {
//				noteToPlay = currentSong;
//			} else if(noteToPlay->time == currentTime) {
//				// TODO: replace this with an edit to the solenoid_states[] array
//				//setKeyOutput(noteToPlay->key, keyOutput);
//				previousNotesPlayed[noteToPlay->key] = (currentTime + noteToPlay->intensity) % SONG_LENGTH;
//				noteToPlay = noteToPlay->next;
//			}
//		}
//		
//		
//		// increment the time
//		currentTime++;
//		if(currentTime > SONG_LENGTH)
//			currentTime = 0;
//		
//		// reset key cool down if it is time too
//		for(i = 0; i < KEYS; i++) {
//			if(keyInputCoolDown[i] == currentTime)
//				keyInputCoolDown[i] = -1;
//		}
//		
//		// TODO: replace with solenoid_play
//		//clock_out(GPIOC, SOL_SR_CLOCK, SOL_SR_DATA, SOL_SR_LATCH, KEY_BYTE_SIZE, keyOutput);
//		
//		// TODO: replace this with a reliable 1-second tick (implemented by a timer)
//		// delay
//		HAL_Delay(1);
//	}
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
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
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

