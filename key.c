#include "key.h"
#include "globals.h"

void insert_note(Song *song, Note note) {
    Song *cur = song;
    Song *after = malloc(sizeof(Song));
    Song *toadd = malloc(sizeof(Song));
    toadd->note = note;

    while(cur != NULL) {
        if(cur->note.duration > note.duration) {
            break;
        }
        if(cur->note.duration < note.duration) {
            after = cur;
        }
        cur = cur->next;
    }

    toadd->next = cur;
    after->next = toadd;
    free(cur);
    free(after);
    free(toadd);
}

#if SDL_PROGRAM

void play_track(Song *song, SDLKey *keys, int delay) {
    Song *cur = song;
    int counter = 0;

    while(cur != NULL) {
        keys[cur->note.key].color = 0xFFFFFF;
        while(counter < delay) {
            counter++;
        }
        counter = 0;
        cur = cur->next;
    }
}

#endif // SDL_PROGRAM
