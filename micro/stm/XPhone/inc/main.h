#ifndef MAIN_H_DEF
	#define MAIN_H_DEF
	
	// primary things about hardware
	#define STM_PROGRAM 	1				// define STM_PROGRAM if it should compile for an stm32 microprocessor for the actual XPhone hardware.
	#define KEYS 			((KeyType)37)	// this is the total number of keys on the XPhone hardware operated by the stm32
	#define MIDI_OFFSET 	((KeyType)60)	// the xylophone starts on middle C, and goes up from there.
	
	
	//==============================================================================
	// clock system
	//==============================================================================
	// 
	#define CPU_FREQ ((uint32_t)100000000)			// 100 MHz (10 ns timebase)
	

#endif // MAIN_H_DEF
