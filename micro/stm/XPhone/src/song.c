#include "song.h"
#include "debug.h"
#include "UART.h"

//=============================================================================
// This sets up all the song stuff.
//=============================================================================
void song_init()
{
	// song variables
	SongTime = 0;										// start out at 0 ms.
	SongLength = KEY_TIME_MAX;							// by default, the song is YEARS long.
	songCurrent = init_note(0, 0, 0);					// create a new note to start the song.
	key_make_track_empty(songCurrent);					// indicate the song is currently empty.
	noteToPlay = songCurrent;							// make noteToPlay point at the song.
	SongNotesTotal = 0;									// the initialized song has no notes.
	SongSkipNextNotes = 0;								// don't skip any notes to start.
	
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
	currentTime = KEY_TIME_MAX;			// reset the main.c while(1) loop current timer as well
	noteToPlay = songCurrent;			// set the net note to play to be the first note in the song.
	// todo: figure out if it makes sense to output a pulse on CTRL_OUT_SYNC anytime you reset the song to 0 time. I think it does...
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
	printf("clearing song. SongNotesTotal=%lu.%s",SongNotesTotal,newline);
	// don't accept shitty input.
	if(song == NULL)
	{
		warning("song_clear() was passed a NULL song! Exiting function...");
		return;
	}
	if(song->key == KEY_TRACK_EMPTY)
	{
		printn("nothing to delete. song is empty.");
		return;
	}
	// free all the notes until you get to the end (where the next note becomes NULL).
	Note *cur = song;
	Note *freeme;
	uint32_t note_count = 0;
	while(cur != NULL)
	{
		freeme = cur;			// record the current note that is not NULL
		cur = cur->next;		// make cur point to the next note
		free(freeme);			// delete the note that you know is not NULL
		note_count++;			// record that you deleted one more note.
	}
	if(note_count != SongNotesTotal)
	{
		warning("The number of notes you just freed is not equal to the number of notes you recorded!");
	}
	printf("done. cleared %lu notes.%s",note_count,newline);
	// start the song from scratch - re-init everything
	song_init();
}
