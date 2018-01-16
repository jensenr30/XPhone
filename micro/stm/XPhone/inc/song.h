#ifndef SONG_H_DEF
	#define SONG_H_DEF
	#include "GPIO.h"
	#include "main.h"
	#include "key.h"

	//=============================================================================
	// definitions for song stuff
	//=============================================================================
	// song timer stuff
	#define SONG_TIM TIM3									// The song timer is driven by timer 3 (16-bit timer)
	#define SONG_TIM_FREQ	((uint16_t)10000)				// 10 kHz input freq
	#define SONG_TIM_PSC	(CPU_FREQ/SONG_TIM_FREQ - 1)	// calculate the prescaler
	#define SONG_TIM_ARR	(10 - 1)						// reset to 0 on the 10th value (this will give a period of 1 ms)
	
	// song variables
	volatile KeyTimeType SongTime;							// this is our current position in the song (ms)
	volatile KeyTimeType SongLength;						// this is how long the song is (ms). Defaults to maximum time (4294967295 milliseconds, or about 1.2 hours)
	Note *songCurrent;										// this points to the current song you are playing
	Note *noteToPlay;										// this will point to the next note you are going to play
	
	// TODO implement a way to limit to total possible notes
	
	//=============================================================================
	// song functions
	//=============================================================================
	
	void song_init();
	void song_set_to_beginning();
	void TIM3_IRQHandler(void);
	void song_clear(Note *song);
	
	
#endif // SONG_H_DEF
