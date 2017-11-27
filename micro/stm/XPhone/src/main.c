//==============================================================================
// Includes
//==============================================================================
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo_144.h"
#include <stdlib.h>
#include "main.h"
#include "GPIO.h"
#include "key.h"
#include "debug.h"
#include "solenoid.h"
#include "song.h"

// clock config. function def
static void SystemClock_Config(void);	


//// JP's key input/output definitions
//#define KEY_BYTE_SIZE 1
//#define KEY_COOLDOWN 10
//
//uint8_t keyInput[KEY_BYTE_SIZE];
//int8_t keyInputCoolDown[KEYS];
//uint8_t keyOutput[KEY_BYTE_SIZE];
//
//#define SONG_LENGTH 1000
//int currentTime = 0;
//int totalNotes = 0;
//
//#define size 14
//int i, j;
//int song[size] = {
//			0,1,2,3,4,5,6,7,6,5,4,3,2,1
//};


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
	SongInit();							// set up song stuff
	//-------------------------------------------------------------------------
	// Main Program Loop
	//-------------------------------------------------------------------------
	
	while(1)
	{
		pin_set(DEBUG_GPIO,DEBUG_0,SongTime%2);
	}
//	// Jensen's little loop for testing the solenoid_play() function.
//	KeyStateType i = 0;					// index
//	SolTimType ms2us = 1000;			// milliseconds to microseconds conversion
//	while (1)							// main program loop
//	{
//		if(i >= KEYS) i = 0;			// reset the index if it gets greater than the number of keys we have.
//		solenoid_play(i++,4*ms2us);	// play a key, and increment the index
//		HAL_Delay(1);					// wait a bit
//	}
	
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

