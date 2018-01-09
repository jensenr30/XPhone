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
#include "control.h"

// clock config. function def
static void system_clock_config(void);	


//=============================================================================
// where the program starts
//=============================================================================
int main(void)
{
	KeyType k;
	KeyTimeType currentTime = KeyTimeMax;	// this is used to store the current time of the song. It is initialized to a value that SongTime will never be, so that the while() wait loops fails and sets it right away.
	
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
	ctrl_init();
	song_init();						// set up song stuff
	
	
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
	
//	uint16_t TON  = 2;
//	uint16_t TOFF = 14;
//	uint16_t cycles = 15;
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
	
		//	// jensen's code to test RGB LED.
//	KeyTimeType ct = KeyTimeMax;	// this is used to store the current time of the song. It is initialized to a value that SongTime will never be, so that the while() wait loops fails and sets it right away.
//	while(1)
//	{
//		while(ct == SongTime) {;}			// wait for the 1 millisecond tick
//		ct = SongTime;						// update the currentTime to the SongTime (which is based on the SONG_TIM song timer)
//		if(ct < 500)
//		{
//			pin_on(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);
//		}
//		else if(ct < 1000)
//		{
//			pin_off(CTRL_OUT_LED_R_GPIO,CTRL_OUT_LED_R);
//			pin_on(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);
//		}
//		else if(ct < 1500)
//		{
//			pin_off(CTRL_OUT_LED_G_GPIO,CTRL_OUT_LED_G);
//			pin_on(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B);
//		}
//		else
//		{
//			pin_off(CTRL_OUT_LED_B_GPIO,CTRL_OUT_LED_B);
//		}
//			
//	}

	
	// calibrate all the key amplitudes
	key_cal();
	

	
//	// jensen's little test of all the keys.
//	int i;
//	i = 0;
//	while(1)
//	{
//		solenoid_play(i%KEYS,3000);
//		i+=7;
//		pause(200);
//	}
//	
	
//	uint32_t adder=0, step=1, wait=125;
//	// play all keys in sequence
//	while(1)
//	{
//		for(k=0;k<KEYS;k++)
//		{
//			solenoid_play((k*step)%KEYS,keyIntensityMin[k]+adder);
//			pause(wait);
//		}
//		//adder += 500;
//		pause(1000);
//	}
	
//	// this is jensen's test to check when activating the solenoid of one key sets off a different key.
//	uint16_t a, t, start=1000, stop=4000, step=500, tmax = 50, k2;
//	while(1)
//	{
//		for(k=0; k<KEYS; k++)					// for each key,
//		{
//			for(a=start;a<=stop;a+=step)				// try a few amplitudes
//			{
//				solenoid_play(k,a);						// play the key
//				for(t=0;t<tmax;t++)
//				{
//					keys_read();							// read all keys
//					for(k2=0; k2<KEYS; k2++)				
//					{
//						if(key_input_states[k2] && (k2 != k))	// if any keys are high that are NOT the key we are trying to play,
//						{
//							warning("That should not have happened");	// give a warning.
//						}
//					}
//					pause(1);
//				}
//				pause(167);
//			}
//		}
//	}
	
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
		if(ctrlMode==CTRL_MODE_PLAY || ctrlMode==CTRL_MODE_RECORD)
		{
			if(noteToPlay == NULL)								// if you have reached the end of the song,
				noteToPlay = songCurrent;							// start over from the beginning
			else if(noteToPlay->key == KEY_TRACK_EMPTY)			// if you have an empty track,
				noteToPlay = songCurrent;							// try refreshing it
			else if(noteToPlay->time >= SongLength)				// if the next note should be played AFTER THE SONG ENDS (ridiculous, should never happen)
			{	
				noteToPlay = songCurrent;							// restart the song
				warning("Somehow, you got a note in your song that has a time that is AFTER your song ends!");
			}
			if(songCurrent->key != KEY_TRACK_EMPTY)				// if this is not an empty song
			{
				while( (noteToPlay->time == currentTime) && (noteToPlay != NULL) )				// if the next note to play should be played at the current time,
				{
					solenoid_play(noteToPlay->key,keyIntensityMin[noteToPlay->key]);	// play it  TODO: put in the proper intensity
					noteToPlay = noteToPlay->next;						// move to the next note
					KeyCooldownActive[noteToPlay->key] = 1;				// activate the cooldown for this key
					KeyCooldownTimes[noteToPlay->key] = KEY_COOLDOWN;	// ^
				}
			}
		}
		
		
		//----------------------------------------------------------------------
		// input all keys into key_inputs[] array.
		//----------------------------------------------------------------------
		keys_read();
		
		ctrlKeyHit = CTRL_IN_INACTIVE;	// reset this flag. If the user hits a key, this will be set to CTRL_IN_ACTIVE_NEW.
		// check to see if you need to add any notes to the song
		for (k=0; k < KEYS; k++)
		{
			// if this key is ON and the cooldown period is not active, 
			if (key_input_states[k] && !KeyCooldownActive[k])
			{
				//pin_on(DEBUG_GPIO,DEBUG_0);
				ctrlKeyHit = CTRL_IN_ACTIVE_NEW;				// record that the user hit a key
				// if you are in recording mode,
				if(ctrlMode == CTRL_MODE_ARMED)
				{
					currentTime = 0;			// reset currentTime so that the first note gets placed at the proper time (0).
				}
				if( (ctrlMode == CTRL_MODE_RECORD) || (ctrlMode == CTRL_MODE_ARMED) )
				{
					// add the key to the song
					Note* n = init_note(k, currentTime, 128);	// TODO set a valid amplitude value
					insert_note(&songCurrent, n);
					// activate the cooldown for this key
					KeyCooldownActive[k] = 1;
					KeyCooldownTimes[k] = KEY_COOLDOWN;
				}
			}
		}
		
		//----------------------------------------------------------------------
		// reset key cool down if it is time too
		//----------------------------------------------------------------------
		for(k = 0; k < KEYS; k++)
		{
			if(KeyCooldownTimes[k])		// if there is still cooldown time left,
			{
				KeyCooldownTimes[k]--;		// decrement the cooldown timer.
			}
			else						// if the cooldown is over,
			{
				if(key_input_states[k]==0)	// if the key is actually off
				{
					KeyCooldownActive[k] = 0;	// you can finally deactivate the cooldown period.
				}
			}	
		}
		
		//----------------------------------------------------------------------
		// manage what mode you are in
		//----------------------------------------------------------------------
		ctrl_in_debounce();			// check inputs and debounce them
		ctrl_mode_manage();			// manage/change the mode that the XPhone is in based on events, user input, etc...
		if(ctrlClear == CTRL_IN_ACTIVE_NEW) song_clear(songCurrent);	// if the user wants to clear the song, do it.
		//----------------------------------------------------------------------
		// control LED based on the current mode
		//----------------------------------------------------------------------
		ctrl_LED();
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

