// key.c
//
//
#ifdef SDL_PROGRAM
#include "globals.h"
#endif
#include "key.h"

/// initilize a note
// key is the key to be played
// time is the time to be played at
// intensity is the intensity of the key being played
Note* init_note(KeyType key, KeyTimeType time, KeyIntensityType intensity) {
    Note *note = malloc(sizeof(Note));
    note->key = key;
    note->time = time;
    note->intensity = intensity;
    note->next = NULL;
    return note;
}

/// insert a note into the song
// Note *song is the address of the list to be inserted into
// Note *note is the address of the note to be inserted into the list
void insert_note(Note **song, Note *note) {
    // iteration variables
    Note *cur = *song;
    Note *after = NULL;

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

/// clear a song
// Note *song is the song to be cleared
void clear_song(Note *song) {
    // if the song does not have any elements don't clear it
    if(song->key == KEY_TRACK_EMPTY)
        return;
    // iteration variable
    Note *cur = song;
    // temporary note variable
    Note *next = NULL;
    // iterate through the list
    while(cur != NULL) {
        // store the next element in a temporary variable
        next = cur->next;
        // remove the current note
        //free(cur);
        cur = NULL;
        // set the current to the next element
        cur = next;
    }
    // set the first element to the empty note
    *song = *init_note(KEY_TRACK_EMPTY, 0, 100);
}

#if STM_PROGRAM
	void key_init()
	{
		KeyType i;
		for(i = 0; i < KEYS; i++) KeyCooldownActive[i] = 0; // initialize the cooldown periods to 0.
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
