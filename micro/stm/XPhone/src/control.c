#include "control.h"

void ctrl_init(){
		ctrlMode = CTRL_MODE_STOP;
		ctrlModeTimeAdder = 0;
		
		ctrlArm = CTRL_IN_INACTIVE;
		ctrlArmDBC = 0;
		ctrlPedal = CTRL_IN_INACTIVE;
		ctrlPedalDBC = 0;
		ctrlKeyHit = CTRL_IN_INACTIVE;
		ctrlCal = CTRL_IN_INACTIVE;
		ctrlCalDBC = 0;
	}
	
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
		
		// if the cal button is held down (active low)
		if(!pin_read(CTRL_IN_CAL_GPIO,CTRL_IN_CAL))
		{
			if(ctrlCalDBC < CTRL_CAL_HOLD_TIME)	ctrlCalDBC++;
			else if(ctrlCal==CTRL_IN_INACTIVE)	ctrlCal = CTRL_IN_ACTIVE_NEW;
			else								ctrlCal = CTRL_IN_ACTIVE_OLD;
		}
		else
		{
			ctrlCalDBC = 0;
			ctrlCal = CTRL_IN_INACTIVE;
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
				ctrl_mode_set(CTRL_MODE_RECORD);										// and start recording
			}
			break;
		case CTRL_MODE_RECORD:													// RECORD mode
			if( (ctrlArm==CTRL_IN_ACTIVE_NEW) || (ctrlPedal==CTRL_IN_ACTIVE_NEW) )	// if either the pedal or the arm button are activated by user,
			{
				ctrl_mode_set(CTRL_MODE_PLAY);											// stop recording; exit to play mode.
				if(SongLength==KeyTimeMax)												// if this is the first thing you recorded,
				{
					SongLength = SongTime;													// set this as the song length.
				}
			}
			break;
		case CTRL_MODE_PLAY:													// PLAY mode
			ctrl_LED_g();
			if( (ctrlArm==CTRL_IN_ACTIVE_NEW) || (ctrlPedal==CTRL_IN_ACTIVE_NEW) )	// if either the pedal or the arm button are activated by user,
			{
				ctrl_mode_set(CTRL_MODE_RECORD);										// start recording use input again.
			}
			break;
		case CTRL_MODE_CAL:
			if( (ctrlArm==CTRL_IN_ACTIVE_NEW) || (ctrlPedal==CTRL_IN_ACTIVE_NEW) || (ctrlCal==CTRL_IN_ACTIVE_NEW) )	// if the user hits the arm button, steps on the pedal, or hits the cal button again,
			{
				ctrl_mode_set(CTRL_MODE_PLAY);										// we exit to PLAY mode.
			}
			break;
		default:
			warning("I have no clue what mode you are in man. It's not on the list.");
			break;
		}
		/// enter cal mode if the user wants to.
		if(ctrlCal==CTRL_IN_ACTIVE_NEW)
		{
			ctrlMode = CTRL_MODE_CAL;
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
		else if(ctrlMode == CTRL_MODE_CAL)
		{
			ctrl_LED_y();
		}
		else												// if you don't know what mode you just got into,
		{
			warning("You are in an unknown mode! WTF?!?!");		// make the LED white.
			ctrl_LED_w();
		}
	}
