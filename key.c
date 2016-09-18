#include "key.h"
#include "globals.h"

void insert_note(Note *song, Note *note) {
    Note *cur = song;
    Note *after = malloc(sizeof(Note));

    while(cur != NULL) {
        if(cur->time > note->time) {
            break;
        }
        if(note->time < note->time) {
            after = cur;
        }
        cur = cur->next;
    }

    note->next = cur;
    after->next = note;
    free(cur);
    free(after);
}

#if SDL_PROGRAM

void play_track(Note *song, SDLKey *keys) {
    Note *cur = song;

    while(cur != NULL) {
        keys[cur->key].color = 0xFFFFFF;
        cur = cur->next;
    }

    free(cur);
}

#endif // SDL_PROGRAM
