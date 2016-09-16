#ifndef KEY_H_INCLUDED
#define KEY_H_INCLUDED

#include <SDL2/SDL.h>

///Structures

/// SDLKey structure
// the key to be rendered
typedef struct {
    // holds a rectangle for the rendering
    // a rectangle or "a key"
    SDL_Rect rect;
    // the color of the rect or "the key"
    int color;
} SDLKey;

/// Note structure
// the note to be played
typedef struct {
    // the key to be played
    unsigned char key;
    // the duration between the start of the track and
    // when the key will be played
    unsigned short duration;
    // how hard the key is going to be hit
    unsigned char intensity;
} Note;

/// Song Linked List structure
// the song to be played
typedef struct Song {
    // the note to be played
    Note note;
    // pointer to the next element in the list
    struct Song *next;
} Song;

/// Functions

void insert_note(Song *song, Note note);
void play_track(Song *song, SDLKey keys[], int delay);


#endif // KEY_H_INCLUDED
