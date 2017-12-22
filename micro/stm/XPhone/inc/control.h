// the XPhone boots up in STOP mode.
// If the user holds the ARM button for a bit of time, the XPhone enters ARMED mode.
// In ARMED mode, the XPhone is waiting for one of three events:
// - if a key is hit, 				the SongTime is set to 0, and the mode is changed to RECORD.
// - if the pedal is stepped on, 	the SongTime is set to 0, and the mode is changed to RECORD.
// - if the ARM button is held for a bit of time again, this will disarm the XPhone. It will return to the last mode it was in (either STOP or PLAY).
// In RECORD mode, the XPhone keeps track of keys hit and saves them in the songCurrent.
// The XPhone will exit RECORD mode into PLAY mode. This happens one of two ways:
// - the pedal it stepped on
// - the ARM button is pressed
// When the XPhone is in PLAY mode, it just loops the songCurrent.
// In PLAY mode, if the user holds the ARM button, it will bypass ARMED mode, and enter directly into RECORD mode, allowing the user to play notes over the existing song (adding/layering/building more notes into the existing song interactively).
	
#ifndef CONTROL_H_DEF
	#define CONTROL_H_DEF
	
	#define ctrlType 				uint8_t
	#define CTRL_MODE_STOP			(0)
	#define CTRL_MODE_ARMED			(1)
	#define CTRL_MODE_RECORD		(2)
	#define CTRL_MODE_PLAY			(3)
	#define CTRL_ARM_DBC_TIME		((KeyTimeType)500)	// milliseconds that the use must hold the ARM button down for it to arm the XPhone.
	#define CTRL_PEDAL_DBC_TIME		((KeyTimeType)500)	// milliseconds that the use must hold the ARM button down for it to arm the XPhone.
	#define CTRL_LED_BLINK_PERIOD	((KeyTimeType)500)	// milliseconds it takes for the LED to turn on and off when in a blinking mode (record / armed).

	#define CTRL_IN_INACTIVE		(0)					// this indicates the input is inactive (off).
	#define CTRL_IN_ACTIVE_NEW		(1)					// this indicates the input JUST changed state.
	#define CTRL_IN_ACTIVE_OLD		(2)					// this indicates the input has been active for a while.

	ctrlType ctrlMode = CTRL_MODE_STOP; 				// default mode is stopped.
	KeyTimeType ctrlModeTimeAdder = 0;					// keeps track of when the last mode change was (useful for making the LED blink ON when the mode change happens - this gives VERY fast visual feedback.
	
	
	uint8_t ctrlArm = 0;								// this tells us the user intended to press the arm button.
	KeyTimeType ctrlArmDBC = 0;							// this is the debounce counter for the arm control input.
	uint8_t ctrlPedal = 0;								// this tells us when the user intended to step on the floor pedal.
	KeyTimeType ctrlPedalDBC = 0;						// this id the debounce counter for the floor pedal.
	uint8_t ctrlKeyHit = 0;								// this tells us if a key was just hit by the user.
	
	void ctrl_mode_set(ctrlType mode)
	{
		ctrlMode = mode;
		ctrlModeTimeAdder = ((SongTime%CTRL_LED_BLINK_PERIOD)+1)*CTRL_LED_BLINK_PERIOD - SongTime;
	}
	
	
	// run this exactly once for every ms of the song
	void ctrl_in_debounce()
	{
		// if the arm button is being held and the counter is not at its max,
		if(pin_read(CTRL_IN_ARM_GPIO,CTRL_IN_ARM))
		{
			if(ctrlArmDBC >= CTRL_ARM_DBC_TIME)	// if the debounce time has elapsed
			{
				if(ctrlArm == CTRL_IN_INACTIVE)		// if the arm button was just pressed,
					ctrlArm = CTRL_IN_ACTIVE_NEW;			// record it as active new.
				else									// otherwise,
					ctrlArm = CTRL_IN_ACTIVE_OLD;			// record it as active old (it's been pressed down for a while).
			}
		}
		else
		{
			if(ctrlArm != CTRL_IN_INACTIVE) ctrlArmDBC = 0;	// reset the debounce timer if necessary
			ctrlArm = CTRL_IN_INACTIVE;			// set the arm button as inactive
			if(ctrlArmDBC < CTRL_ARM_DBC_TIME)	// if the debounce time has not yet been met,
				ctrlArmDBC++;							// increment the debounce time
		}
		
		// if the pedal is held down
		if(!pin_read(CTRL_IN_PEDAL_GPIO,CTRL_IN_PEDAL))
		{
			if(ctrlPedalDBC >= CTRL_PEDAL_DBC_TIME)	// if the debounce time has elapsed
			{
				if(ctrlPedal == CTRL_IN_INACTIVE)		// if the pedal was just hit,
					ctrlPedal = CTRL_IN_ACTIVE_NEW;			// record it as active new.
				else									// otherwise,
					ctrlPedal = CTRL_IN_ACTIVE_OLD;			// record it as active old (it's been held for a while).
			}
		}
		else
		{
			if(ctrlPedal != CTRL_IN_INACTIVE) ctrlPedalDBC = 0;	// reset the debounce timer if necessary
			ctrlPedal = CTRL_IN_INACTIVE;			// set the pedal as inactive
			if(ctrlPedalDBC < CTRL_PEDAL_DBC_TIME)	// if the debounce time has not yet been met,
				ctrlPedalDBC++;							// increment the debounce time
		}
	}
	
	// run this as frequently as you like. I recommend once per ms of the song.
	void ctrl_mode_manage()
	{
		switch(ctrlMode)
		{
		case CTRL_MODE_STOP:													// STOP mode
			if( (ctrlArm==CTRL_IN_ACTIVE_NEW) || (ctrlPedal==CTRL_IN_ACTIVE_NEW) )	// if either the pedal or the arm button are activated by user,
			{
				ctrl_mode_set(CTRL_MODE_ARMED);											// move into armed mode
			}
			break;
		case CTRL_MODE_ARMED:													// ARMED mode
			if( (ctrlPedal==CTRL_IN_ACTIVE_NEW) || (ctrlKeyHit==CTRL_IN_ACTIVE_NEW) || (ctrlArm==CTRL_IN_ACTIVE_NEW) ) // if [the pedal was just stepped on] OR [a key was just hit] OR [the arm button was just pressed],  
			{
				song_set_to_beginning();												// set song to beginning
				ctrl_mode_set(CTRL_MODE_RECORD);										// and start recording
			}
			break;
		case CTRL_MODE_RECORD:													// RECORD mode
			if( (ctrlArm==CTRL_IN_ACTIVE_NEW) || (ctrlPedal==CTRL_IN_ACTIVE_NEW) )	// if either the pedal or the arm button are activated by user,
			{
				ctrl_mode_set(CTRL_MODE_PLAY);											// stop recording; exit to play mode.
			}
			break;
		case CTRL_MODE_PLAY:													// PLAY mode
			ctrl_LED_g();
			if( (ctrlArm==CTRL_IN_ACTIVE_NEW) || (ctrlPedal==CTRL_IN_ACTIVE_NEW) )	// if either the pedal or the arm button are activated by user,
			{
				ctrl_mode_set(CTRL_MODE_RECORD);										// start recording use input again.
			}
			break;
		default:
			warning("I have no clue what mode you are in man. It's not on the list.");
			break;
		}
		
	}
	
	
	void ctrl_LED()
	{
		if(ctrlMode == CTRL_MODE_STOP)			// if you are in the STOP mode,
		{
			ctrl_LED_o();							// make the LED off.
		}
		else if(ctrlMode == CTRL_MODE_PLAY)		// if you are in the PLAY mode,
		{
			ctrl_LED_g();							// make the LED green.
		}
		else if(ctrlMode == CTRL_MODE_ARMED)	// if you are in the ARMED mode,
		{
													// make the LED blink blue.
			if( ((SongTime+ctrlModeTimeAdder)%(CTRL_LED_BLINK_PERIOD)) < (CTRL_LED_BLINK_PERIOD/2) )
			{
				ctrl_LED_b();
			}
			else
			{
				ctrl_LED_o();
			}
		}
		else if(ctrlMode == CTRL_MODE_RECORD)	// if you are in the RECORD mode,
		{
													// make the LED blink red.
			if( ((SongTime+ctrlModeTimeAdder)%(CTRL_LED_BLINK_PERIOD)) < (CTRL_LED_BLINK_PERIOD/2) )
			{
				ctrl_LED_r();
			}
			else
			{
				ctrl_LED_o();
			}
		}
		else												// if you don't know what mode you just got into,
		{
			warning("You are in an unknown mode! WTF?!?!");		// make the LED white.
			ctrl_LED_w();
		}
	}
	
	
#endif
