// key.c
#ifdef SDL_PROGRAM
	#include "globals.h"
#endif
#include "key.h"
#include "solenoid.h"
#include "amux.h"
#include "GPIO.h"
#include <stdlib.h>
#include "song.h"
#include "debug.h"
#include "UART.h"

/// initilize a note
// key is the key to be played
// time is the time to be played at
// intensity is the intensity of the key being played
Note* init_note(KeyType key, KeyTimeType time, KeyIntensityType intensity) {
    Note *note = malloc(sizeof(Note));
    if(note == NULL)
    {
    	warning("Could not init_note()! We ran out of memory, Jim!");
    	return NULL;
    }
    note->key = key;
    note->time = time;
    note->intensity = intensity;
    note->next = NULL;
    return note;
}

// change a note to signify an empty song.
void key_make_track_empty(Note *note)
{
	if(note == NULL)
	{
		warning("key_make_track_empty() received NULL note. Exiting function...");
		return;
	}
	note->intensity = 69;			// a way to remember the note was created in this function
	note->key = KEY_TRACK_EMPTY;	// this note tells you the song is empty.
	note->next = NULL;				// there is no note after this one.
	note->time = 0;					// this doesn't really matter.
}

/// insert a note into the song
// Note *song is the address of the list to be inserted into
// Note *note is the address of the note to be inserted into the list
void insert_note(Note **song, Note *note) {
	
	if(note == NULL)
	{
		warning("insert_note(): note = NULL. aborting");
		return;
	}
	if(song == NULL)
	{
		warning("insert_note(): song = NULL. aborting");
		return;
	}
	
	// if this is the first note to insert,
	if( (*song)->key == KEY_TRACK_EMPTY )
	{
		note_clear_from_memory(*song);			// delete the placeholder note,
		*song = note;							// point the song to the current note.
		noteToPlay = *song;						// the next note you should play is the first one (when you stop recording and start playing this note)
	}
	// if this is NOT the first note to insert,
	else
	{
		
		Note *cur = *song;		// iteration variables
		Note *after = NULL;		// the new *note will be inserted just after this one.
		
		// iterate over all the elements
		while(cur != NULL) {
			// check if the note should go before the current note
			if(cur->time > note->time) {
				break;
			}
			// check if the note should go after the current note
			if(cur->time < note->time) {
				// save the note for later use
				after = cur;
			}
			// get next element
			cur = cur->next;
		}
	
		// check if it is the first element of the song to be added onto
		if(after == NULL) {
			note->next = *song;
			*song = note;
		// check if it is the first note to be added to the song
		} else if(after->key == KEY_TRACK_EMPTY) {
			// set note as first element
			*song = note;
		// check if it is the last item to be added
		} else if(cur == NULL) {
			after->next = note;
		// add if between two items already in the list
		} else {
			after->next = note;
			note->next = cur;
		}
	}
}


void note_delete(Note **song, Note *note)
{
	if(note == NULL)
	{
		warning("note_delete(): note = NULL. aborting");
		return;
	}
	if(song == NULL)
	{
		warning("note_delete(): song = NULL. aborting");
		return;
	}
	
	printf("note_delete(): note->key = %ud, note->time = %.3f ms.%s",note->key,SongTimeSec(note->time),newline);
	
	// if the note you want to delete is the first note of the song,
	if(*song == note)
	{
		note = note->next;				// grab the second note in the song
		note_clear_from_memory(*song);	// free the first note in the song
		SongNotesTotal--;				// remember to record that you deleted one.
		*song = note;					// make the note start at the second note in the song (which is now the start of the new song).
		return;
	}
	
	Note *last_note = *song;
	Note *cur = (*song)->next;
	uint8_t done = 0;
	uint8_t foundNote = 0;
	while(!done)
	{
		// if the current note is NULL you have to stop, even though you didn't find the right note :/
		if(cur == NULL)
		{
			foundNote = 0;
			break;
		}
		// if you found the note you want to delete
		if(cur == note)
		{
			last_note->next = cur->next;	// make the song skip this one
			note_clear_from_memory(cur);	// delete the note you wanted to delete.
			SongNotesTotal--;				// remember to record that you deleted one.
			done = 1;
			foundNote = 1;
		}
		// otherwise, if this was not the note you wanted to delete,
		else
		{
			last_note = cur;		// save the current note as the next iteration's last_note.
			cur = cur->next;		// move on to the next note
		}
	}
	if(!foundNote)
	{
		warning("note_delete(): couldn't find the right note to delete");
	}
}


// this function is not optimal. replaced by: song.c -> song_clear()
///// clear a song
//// Note *song is the song to be cleared
//void clear_song(Note *song) {
//    // if the song does not have any elements don't clear it
//    if(song->key == KEY_TRACK_EMPTY)
//        return;
//    // iteration variable
//    Note *cur = song;
//    // temporary note variable
//    Note *next = NULL;
//    // iterate through the list
//    while(cur != NULL) {
//        // store the next element in a temporary variable
//        next = cur->next;
//        // remove the current note
//        //free(cur);					// TODO need to put this back into the program, probably use the note_clear_from_memory function.
//        cur = NULL;
//        // set the current to the next element
//        cur = next;
//    }
//    // set the first element to the empty note
//    *song = *init_note(KEY_TRACK_EMPTY, 0, 100);
//}


void note_clear_from_memory(Note *note)
{
    if(note == NULL)
    {
    	warning("Could not clear note from memory using note_clear_from_memory()! Reason: I got a NULL pointer!");
    	return;
    }
    free(note);
}


void key_step_intensity(SolTimType start, SolTimType stop, SolTimType step)
{
	KeyType key, k;
	float voltages[KEYS];
	printf("intensity, ");
	for(k=0; k<KEYS; k++)
	{
		printf("key %d",k);
		if(k != (KEYS-1)) printf(", ");
	}
	printf(newline);
	
	SolTimType i;
	for(i=start; i<=stop; i+=step){
		printf("%lu, ",i);
		for(key=0; key<KEYS; key++)
		{
			k = (key*step)%KEYS;											// step through the keys in a tasteful way
			solenoid_play(k,i);												// play this key
			pause_ms(AMUX_SAMPLE_HOLDOFF);									// wait the appropriate holdoff time
			voltages[k] = ADC_volt(amux_read(k));							// record the voltage produced by the key circuitry
			
			pause_ms(50);
		}
		
		for(k=0; k<KEYS; k++)
		{
			printf("%.2f",voltages[k]);
			if(k != (KEYS-1)) printf(", ");
		}
		printf(newline);
	}
}



void key_cal_default()
{
	keyIntensityMin[0]	=	2822;
	keyIntensityMin[1]	=	2618;
	keyIntensityMin[2]	=	3009;
	keyIntensityMin[3]	=	2710;
	keyIntensityMin[4]	=	2499;
	keyIntensityMin[5]	=	3231;
	keyIntensityMin[6]	=	2419;
	keyIntensityMin[7]	=	2674;
	keyIntensityMin[8]	=	3102;
	keyIntensityMin[9]	=	3581;
	keyIntensityMin[10]	=	2962;
	keyIntensityMin[11]	=	2481;
	
	keyIntensityMin[12]	=	2309;
	keyIntensityMin[13]	=	2606;
	keyIntensityMin[14]	=	2993;
	keyIntensityMin[15]	=	2990;
	keyIntensityMin[16]	=	2227;
	keyIntensityMin[17]	=	2854;
	keyIntensityMin[18]	=	1917;
	keyIntensityMin[19]	=	3066;
	keyIntensityMin[20]	=	3286;
	keyIntensityMin[21]	=	1994;
	keyIntensityMin[22]	=	2174;
	keyIntensityMin[23]	=	2179;
	
	keyIntensityMin[24]	=	2669;
	keyIntensityMin[25]	=	3172;
	keyIntensityMin[26]	=	3641;
	keyIntensityMin[27]	=	4149;
	keyIntensityMin[28]	=	4222;
	keyIntensityMin[29]	=	4817;
	keyIntensityMin[30]	=	3009;
	keyIntensityMin[31]	=	3242;
	keyIntensityMin[32]	=	2618;
	keyIntensityMin[33]	=	3069;
	keyIntensityMin[34]	=	3428;
	keyIntensityMin[35]	=	2729;
	
	keyIntensityMin[36]	=	3004;
}


// calibrates all the keys on the XPhone
void key_cal()
{
	KeyType key, k;
	uint8_t c, cycles = 6, step = 5;
	float voltages[KEYS];
	char buffer[100];
	printn("cal_cycle,key,solenoid_time_us,voltage_V");
	for(c=0; c<cycles; c++)
	{
		for(key=0; key<KEYS; key++)
		{
			k = (key*step)%KEYS;											// step through the keys in a tasteful way
			solenoid_play(k,keyIntensityMin[k]);							// play this key
			pause_ms(AMUX_SAMPLE_HOLDOFF);									// wait the appropriate holdoff time
			voltages[k] = ADC_volt(amux_read(k));										// record the voltage produced by the key circuitry
			sprintf(buffer, "%2d, %2d ,%4lu, %.2f",c,k,keyIntensityMin[k],voltages[k]);	// tell the user what you are doing.
			printf("%s%s",buffer,newline);
			keyIntensityMin[k] += (int16_t)((KEY_CAL_TARGET - voltages[k])*key_cal_adj(k));	// adjust the amount of time you turn the solenoid on for this key.
		}
	}

	
//	// first attempt at calibration method. Doesn't work so well because the sensitivity of the keys is not really the same across all keys.
//	ctrl_LED_w();
//	uint8_t gotMin;
//	uint16_t tmax = 20;	// max amount of time you should have to wait to see the results of your xylophone key hit is this many ms.
//	uint16_t t;
//	KeyType k;
//	for(k=0; k<KEYS; k++)
//	{
//		gotMin = 0;
//		keyIntensityMin[k] = KEY_CAL_START;
//		while(!gotMin)
//		{
//			solenoid_play(k,keyIntensityMin[k]);
//			t = 0;
//			while(t<tmax)
//			{
//				pause_ms(1);
//				keys_read();
//				gotMin |= key_input_states[k];
//				t++;
//			}
//			if(!gotMin) keyIntensityMin[k] += KEY_CAL_STEP;
//			pause_ms(KEY_CAL_STEP_TIME);
//		}
//		
//	}
}

void keys_read()
{
	shift_in(KEY_INPUT_GPIO, KEY_INPUT_CLOCK, KEY_INPUT_DATA, KEY_INPUT_LATCH, KEYS, (KeyStateType *) key_input_states, KEY_INPUT_DIR);
}

#if STM_PROGRAM
	void key_init()
	{
		KeyType i;
		for(i = 0; i < KEYS; i++)
		{
			KeyCooldownActive[i] = 0;	// initialize all cooldowns to be not active at the beginning.
			KeyCooldownTimes[i] = 0;	// initialize all the cooldown times to 0.
			key_input_states[i] = 0;	// initialize the key input states to 0.
		}
	}
#endif


#if SDL_PROGRAM

/// save song to a text file
// Note *song is the song to be saved
// returns 1 if it succeeded or 0 if it failed
uint8_t save_song(Note* song, char *name) {
    // iteration variable
    Note *cur = song;
    // open file
    FILE *fp = fopen(name, "w");
    // check dat pointer
    if(fp == NULL) {
        // failed to write
        return 0;
    }

    // iterate through the list
    while(cur != NULL) {
        //write key data to file
        fprintf(fp, "key: %i ", cur->key);
        fprintf(fp, "time: %i ", cur->time);
        fprintf(fp, "intensity: %i\n", cur->intensity);
        // store the next element in a temporary variable
        cur = cur->next;
    }

    // close dat file
    fclose(fp);
    // save successful
    return 1;
}

/// play the current song
// Note* song is the address to the song to be played
// SDLKey *key is the address to the key structure used for rendering a change to the keys
void play_song(Note *song, SDLKey *keys) {
    // if the song does not have any elements don't play it
    if(song->key == KEY_TRACK_EMPTY)
        return;
    // iteration variable
    Note *cur = song;
    // iterate through the list
    while(cur != NULL) {
        // set the color of the key to be rendered to white
        keys[cur->key].color = 0xFFFFFF;
        // get next element
        cur = cur->next;
    }
}

#endif // SDL_PROGRAM
