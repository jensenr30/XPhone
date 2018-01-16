#include "solenoid.h"

//==============================================================================
// initialize all solenoid-related things
//==============================================================================
void solenoid_init()
{
	//--------------------------------------------------------------------------
	// set initial states of the solenoid shift-register pins
	//--------------------------------------------------------------------------
	pin_off(SOL_SR_CLOCK_GPIO, SOL_SR_CLOCK);
	pin_off(SOL_SR_DATA_GPIO, SOL_SR_DATA);
	pin_off(SOL_SR_LATCH_GPIO, SOL_SR_LATCH);
	
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
	solenoid_update();
	
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
	if(keys_all_off) SOL_TIM->CNT = 0;				// if all the keys are off now, it is a good time to reset the timer to 0. This is done to avoid the overflow problem. The timer will always be reset long before we run out of time.
}


//=============================================================================
// timer 2 Interrupt Handler
// benchmark time = 43.6 us with 37 keys.
//=============================================================================
void TIM2_IRQHandler(void)
{
	
//		// this is for catching the interrupt that is generated when the value gets updated (rollover, auto-reload, modify, etc.)
//		// this is unnecessary for the time being because I only need to interrupt on the CCR1 register (below)
//		if (TIM2->SR & TIM_SR_UIF) // if UIF flag is set
//		{
//		TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag
//		}
	
	if (SOL_TIM->SR & TIM_SR_CC1IF) 				// if the counter compare flag is set,
	{
		SOL_TIM->SR &= ~TIM_SR_CC1IF;					// clear it.
		//pin_on(DEBUG_GPIO,DEBUG_1);
		solenoid_shut_off_the_right_ones();				// shut off the right solenoids.
		//pin_off(DEBUG_GPIO,DEBUG_1);
		solenoid_interrupt_recalculate();				// calculate when the next interrupt should be.
		solenoid_update();								// update the states of the solenoids
	}
	
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
		#if(DEBUG_UART)
			printf("key=%d  length=%.3f ms%s",key, length/(float)1000,newline);
		#endif
	}
	if (length >= SOL_TIME_TOO_LONG)
	{
		warning("you are trying to play a key for too long. I'll still play the key, but there is probably an error in the code that is making it so long...");
		length = SOL_TIME_TOO_LONG;
		#if(DEBUG_UART)
			printf("key=%d  length=%.3f ms%s",key, length/(float)1000,newline);
		#endif
	}
	if (length <= SOL_TIME_TOO_SHORT)
	{
		warning("you are trying to play a key for too short. I'll still play the key (at the minimum length), but there is probably an error in the code (or calibration data) that is making it so short...");
		length = SOL_TIME_TOO_SHORT;
		#if(DEBUG_UART)
			printf("key=%d  length=%.3f ms%s",key, length/(float)1000,newline);
		#endif
	}
	if (solenoid_states[key])
	{
		warning("You try to play key that is already on!");
		#if(DEBUG_UART)
			printf("key=%d  length=%.3f ms%s",key, length/(float)1000,newline);
		#endif
		return;
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
