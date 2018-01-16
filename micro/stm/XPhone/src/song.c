#include "song.h"
#include "debug.h"

//=============================================================================
// This sets up all the song stuff.
//=============================================================================
void song_init()
{
	// song variables
	SongTime = 0;										// start out at 0 ms.
	SongLength = KeyTimeMax;							// by default, the song is YEARS long.
	songCurrent = init_note(0, 0, 0);					// create a new note to start the song.
	key_make_track_empty(songCurrent);					// indicate the song is currently empty.
	noteToPlay = songCurrent;							// make noteToPlay point at the song.
	
	// song 1-ms timer setup
	__HAL_RCC_TIM3_CLK_ENABLE();
	SONG_TIM->PSC = SONG_TIM_PSC;	 					// Set prescaler. Division factor = (PSC + 1)
	SONG_TIM->ARR = SONG_TIM_ARR;						// Auto reload value. period = (ARR + 1)
	SONG_TIM->DIER = TIM_DIER_UIE;						// Enable update interrupt (timer level)
	SONG_TIM->CR1 = TIM_CR1_CEN;						// Enable timer
	NVIC_EnableIRQ(TIM3_IRQn);							// enable timer interrupt handler
	SONG_TIM->CNT = 0;									// init the counter at 0.
	SONG_TIM->EGR |= TIM_EGR_UG;						// generate a timer update (this updates all the timer settings that were just configured). See RM0402.pdf section 17.4.6	"TIMx event generation register (TIMx_EGR)"
}


//=============================================================================
// restarts the song timer to 0
//=============================================================================
void song_set_to_beginning()
{
	SONG_TIM->CNT = 0;					// set the timer/counter to 0 (this ensures that the first ms of the song really lasts the full ms).
	SongTime = 0;						// set the song time to 0
	currentTime = 0;					// reset the main.c while(1) loop current timer as well
	noteToPlay = songCurrent;			// set the net note to play to be the first note in the song.
}


//=============================================================================
// this function increments the SongTime variable once for every millisecond that passes.
// At 1 second, SongTime will equal 1000.
//=============================================================================
void TIM3_IRQHandler(void)
{
	if (SONG_TIM->SR & TIM_SR_UIF) 				// if UIF flag is set
	{
		SONG_TIM->SR &= ~TIM_SR_UIF;				// clear UIF flag
		if(SongTime+1 >= SongLength)				// if the song has reached its end,
		{
			SongTime = 0;								// start over
		}
		else										// otherwise,
		{
			SongTime++;									// increment song time by 1
		}
	}
}


//=============================================================================
// this deletes all the notes from the song you give it (frees memory).
// it will leave the song in the same state song_init() does (note[0] = KEY_TRACK_EMPTY).
//=============================================================================
void song_clear(Note *song)
{
	// don't accept shitty input.
	if(song == NULL)
	{
		warning("song_clear() was passed a NULL song! Exiting function...");
		return;
	}
	// free all the notes until you get to the end (where the next note becomes NULL).
	Note *cur = song->next;
	Note *freeme = NULL;
	while(cur != NULL)
	{
		freeme = cur;
		cur = cur->next;
		free(freeme);
	}
	// indicate the song is now empty by changing the first note to a KEY_TRACK_EMPTY note using the proper function:
	key_make_track_empty(song);
}
