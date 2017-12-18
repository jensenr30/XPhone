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
static void system_clock_config(void);	


// JP's key input/output definitions
int totalNotes = 0;

#define size 14
int i, j;
int song[size] = {
			0,1,2,3,4,5,6,7,6,5,4,3,2,1
};


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
	system_clock_config();				// Configure the system clock to 100 MHz
	GPIO_init();						// set up all GPIO pins for everything.
	solenoid_init();					// initialize all solenoid stuff.
	key_init();							// initialize all the key stuff
	song_init();							// set up song stuff
	
//	// Code to test the 1-ms tick
//	while(1)
//	{
//		pin_set(DEBUG_GPIO,DEBUG_0,SongTime%2);
//	}
	
//	// Jensen's little loop for testing the solenoid_play() function.
//	KeyStateType i = 0;					// index
//	SolTimType ms2us = 1000;			// milliseconds to microseconds conversion
//	while (1)							// main program loop
//	{
//		if(i >= KEYS) i = 0;			// reset the index if it gets greater than the number of keys we have.
//		solenoid_play(i++,4*ms2us);	// play a key, and increment the index
//		HAL_Delay(1);					// wait a bit
//	}
	
	
//	// A little test to make sure shift_in is working.
//	while(1)
//	{
//		shift_in(KEY_INPUT_GPIO,KEY_INPUT_CLOCK,KEY_INPUT_DATA,KEY_INPUT_LATCH,8,(uint8_t *)solenoid_states,1);
//		solenoid_update();
//	}	
	
//	// jensen's code to test hitting a key
//	while (1)
//	{
//		solenoid_play(3,4000);
//		HAL_Delay(200);
//	}
	
//	// jensen's code to test volume vs time 
//	while (1)
//	{
//		solenoid_play(3,3000);
//		HAL_Delay(1000);
//		solenoid_play(3,3500);
//		HAL_Delay(1000);
//		solenoid_play(3,4000);
//		HAL_Delay(1000);
//		solenoid_play(3,5000);
//		HAL_Delay(1000);
//		solenoid_play(3,6000);
//		HAL_Delay(1000);
//		solenoid_play(3,8000);
//		HAL_Delay(1000);
//		solenoid_play(3,9999);
//		HAL_Delay(5000);
//	}
	
	uint16_t TON  = 2;
	uint16_t TOFF = 14;
	uint16_t cycles = 15;
	// code to test solenoid hitting (direct GPIO control. i know i'm using the debug pin. it doesn't make sense, but that is the pin I chose.)
//	while (1)
//	{
//		for (i=0; i<cycles; i++)
//		{
//			pin_on(DEBUG_GPIO,DEBUG_ERROR_LED);
//			HAL_Delay(TON);
//			pin_off(DEBUG_GPIO,DEBUG_ERROR_LED);
//			HAL_Delay(TOFF);
//		}
//		HAL_Delay(1500);
//	}
//	
//	
//	// code to see how many notes I can add before something goes to shit (run out of memory)
//	// last time I checked this, I got a warning on note #10496 (  I can have over 10,000 note song!  =D  this is probably way more than I'll ever need)
//	uint32_t i_i_i = 0;
//	Note* jenNOTE;
//	while(1)
//	{
//		jenNOTE = init_note(i_i_i%256, i_i_i, 40);
//		insert_note(&songCurrent, jenNOTE);
//		i_i_i++;
//	}
	
	
	KeyType k;
	KeyTimeType currentTime = KeyTimeMax;	// this is used to store the current time of the song. It is initialized to a value that SongTime will never be, so that the while() wait loops fails and sets it right away.
	//-------------------------------------------------------------------------
	// Main Program Loop
	//-------------------------------------------------------------------------
	while (1)
	{
		while(currentTime == SongTime) {;}			// wait for the 1 millisecond tick
		currentTime = SongTime;						// update the currentTime to the SongTime (which is based on the SONG_TIM song timer)
		
		//----------------------------------------------------------------------
		// play all the notes you must
		//----------------------------------------------------------------------
		if(noteToPlay == NULL)								// if you have reached the end of the song,
			noteToPlay = songCurrent;							// start over from the beginning
		else if(noteToPlay->key == KEY_TRACK_EMPTY)			
			noteToPlay = songCurrent;							// try refreshing it
		else if(noteToPlay->time >= SongLength)				// if the next note should be played AFTER THE SONG ENDS (ridiculous, should never happen)
		{	
			noteToPlay = songCurrent;							// restart the song
			warning("Somehow, you got a note in your song that has a time that is AFTER your song ends!");
		}
		if(songCurrent->key != KEY_TRACK_EMPTY)				// if this is not an empty song
		{
			while(noteToPlay->time == currentTime)				// if the next note to play should be played at the curren time,
			{
				solenoid_play(noteToPlay->key,4000);				// play it  TODO: put in the proper intensity
				noteToPlay = noteToPlay->next;						// move to the next note
				KeyCooldownActive[noteToPlay->key] = 1;				// activate the cooldown for this key
				KeyCooldownTimes[noteToPlay->key] = (currentTime + KEY_COOLDOWN) % SongLength;	// ^
				if(noteToPlay == NULL) break;						// if this is the end of the song, start over.
			}
		}
		
		//----------------------------------------------------------------------
		// input all keys into key_inputs[] array.
		//----------------------------------------------------------------------
		shift_in(KEY_INPUT_GPIO, KEY_INPUT_CLOCK, KEY_INPUT_DATA, KEY_INPUT_LATCH, KEYS, (KeyStateType *) key_input_states, KEY_INPUT_DIR);
		
		// check to see if you need to add any notes to the song
		for (k=0; k < KEYS; k++)
		{
			// if this key is ON and the cooldown period is not active, 
			if (key_input_states[k] && !KeyCooldownActive[k])
			{
				//pin_on(DEBUG_GPIO,DEBUG_0);
				// add the key to the song
				Note* n = init_note(k, currentTime, 40);
				insert_note(&songCurrent, n);
				// activate the cooldown for this key
				KeyCooldownActive[k] = 1;
				KeyCooldownTimes[k] = (currentTime + KEY_COOLDOWN) % SongLength;
			}
		}
		
		//----------------------------------------------------------------------
		// reset key cool down if it is time too
		//----------------------------------------------------------------------
		for(k = 0; k < KEYS; k++)
		{
			if(KeyCooldownTimes[k] == currentTime)		// if the cooldown is over, 
				KeyCooldownActive[k] = 2;					// flag that the cooldown is over.
			if( (KeyCooldownActive[k] == 2) && (key_input_states[k] == 0) )		// but wait until the key input is actually LOW...
				KeyCooldownActive[k] = 0;					// ...before you actually accept another key press
		}
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
static void system_clock_config(void)
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

