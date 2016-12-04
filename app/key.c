#include "key.h"
#include "globals.h"

Note* init_note(uint8_t key, uint16_t time, uint8_t intensity) {
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
void insert_note(Note *song, Note *note) {
    // iteration variables
    Note *cur = song;
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

    // check if it is the last element to be added
    if(after == NULL) {
        cur->next = note;
    // check if it is the first element to be added
    } else if(after->key == KEY_TRACK_EMPTY) {
        // set note as first element
        *song = *note;
    // check if it is the last item to be added
    } else if(cur == NULL) {
        after->next = note;
    // add if between two items already in the list
    } else {
        after->next = note;
        note->next = cur;
    }
}

#if SDL_PROGRAM

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
