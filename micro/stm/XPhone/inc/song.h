#ifndef SONG_H_DEF
	#define SONG_H_DEF
	
	//=============================================================================
	// definitions for song stuff
	//=============================================================================
	#define SONG_TIM TIM3									// The song timer is driven by timer 3 (16-bit timer)
	#define SONG_TIM_FREQ	((uint16_t)10000)				// 10 kHz input freq
	#define SONG_TIM_PSC	(CPU_FREQ/SONG_TIM_FREQ - 1)	// calculate the prescaler
	#define SONG_TIM_ARR	(9)								// reset to 0 on the 10th value (this will give a period of 1 ms)
	KeyTimeType SongTime;
	
	void SongInit()
	{
		SongTime = 0;								// start out at 0 ms.
		__HAL_RCC_TIM3_CLK_ENABLE();
		SONG_TIM->PSC = SONG_TIM_PSC;	 			// Set prescaler. Division factor = (PSC + 1)
		SONG_TIM->ARR = SONG_TIM_ARR;				// Auto reload value. period = (ARR + 1)
		SONG_TIM->DIER = TIM_DIER_UIE;				// Enable update interrupt (timer level)
		SONG_TIM->CR1 = TIM_CR1_CEN;				// Enable timer
		NVIC_EnableIRQ(TIM3_IRQn);					// enable timer interrupt handler
		SONG_TIM->CNT = 0;							// init the counter at 0.
		SONG_TIM->EGR |= TIM_EGR_UG;				// generate a timer update (this updates all the timer settings that were just configured). See RM0402.pdf section 17.4.6	"TIMx event generation register (TIMx_EGR)"
	}
	
	//=============================================================================
	// timer 2 Interrupt Handler
	// benchmark time = 43.6 us with 37 keys.
	//=============================================================================
	void TIM3_IRQHandler(void)
	{
		if (SONG_TIM->SR & TIM_SR_UIF) 				// if UIF flag is set
		{
			SONG_TIM->SR &= ~TIM_SR_UIF;				// clear UIF flag
			SongTime++;									// increment song time
		}
	}
	
	
#endif // SONG_H_DEF
