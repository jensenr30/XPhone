#ifndef SONG_H_DEF
	#define SONG_H_DEF
	
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
	volatile KeyTimeType SongLength = KeyTimeMax;			// this is how long the song is (ms). Defaults to maximum time (4294967295 milliseconds, or about 1.2 hours)
	Note *songCurrent = NULL;
	// this will point to the next note you are going to play
	Note *noteToPlay = NULL;
	
	// TODO implement a way to limit to total possible notes
	
	//=============================================================================
	// This sets up all the song stuff.
	//=============================================================================
	void song_init()
	{
		// song variables
		SongTime = 0;										// start out at 0 ms.
		SongLength = 2000;									// by default, the song starts out being 6 second long
		songCurrent = init_note(KEY_TRACK_EMPTY, 0, 100);	// create an empty song
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
		SONG_TIM->CNT = 0;									// set the timer/counter to 0 (this ensures that the first ms of the song really lasts the full ms).
		SongTime = 0;										// set the song time to 0
	}
	
	
	//=============================================================================
	// blocks program execution for some time (milliseconds)
	//=============================================================================
	void pause(uint32_t ms)
	{
		KeyTimeType st = SongTime;
		while(ms)
		{
			while(st == SongTime){}
			st = SongTime;
			ms--;
		}
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
				SongTime = 0;								// start over
			else										// otherwise,
				SongTime++;									// increment song time by 1
		}
	}
	
	
#endif // SONG_H_DEF
