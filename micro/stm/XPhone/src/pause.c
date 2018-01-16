#include "pause.h"

//=============================================================================
// blocks program execution for some time (milliseconds)
//=============================================================================
void pause_ms(uint32_t ms)
{
	KeyTimeType st = SongTime;
	// wait until you have run out of milliseconds.
	while(ms)
	{
		while(st == SongTime){}
		st = SongTime;
		ms--;
	}
}


//=============================================================================
// blocks program execution for some time (microseconds)
//=============================================================================
void pause_us(uint32_t us)
{
	// capture the current value of the solenoid timer
	SolTimType st = SOL_TIM->CNT;
	// wait until you have run out of microseconds
	while(us)
	{
		while(st == SOL_TIM->CNT){}
		st = SOL_TIM->CNT;
		us--;
	}
}
