#ifndef SOLENOID_H_DEF
	#define SOLENOID_H_DEF
	#include <inttypes.h>
	#include "GPIO.h"
	#include "main.h"
	#include "debug.h"
	#include "key.h"
	#include "UART.h"
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
	#define solenoid_update() shift_out(SOL_SR_CLOCK_GPIO,SOL_SR_CLOCK,SOL_SR_DATA_GPIO,SOL_SR_DATA,SOL_SR_LATCH_GPIO,SOL_SR_LATCH,KEYS,(KeyStateType *)solenoid_states,SOL_SR_DIR)
	
	#if(SOL_TIM_OFF < SOL_TIM_MOD)
	#error "You cannot make your solenoid OFF flag be a value less than the solenoid modulo value."
	#endif
	
	
	//==============================================================================
	// solenoid functions
	//==============================================================================
	void solenoid_init();
	void solenoid_interrupt_recalculate();
	void solenoid_shut_off_the_right_ones();
	void TIM2_IRQHandler(void);
	void solenoid_play(KeyType key, SolTimType length);

#endif // SOLENOID_H_DEF
