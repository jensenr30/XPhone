#ifndef KEY_H_INCLUDED
#define KEY_H_INCLUDED

#include <SDL2/SDL.h>


/// Structures



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
// the notes to be played
typedef struct Note{
    // the key to be played
    uint8_t key;
    // the time between the start of the track and
    // when the key will be played (in milliseconds)
    uint16_t time;
    // how hard the key is going to be hit
    uint8_t intensity;
    // pointer to the next element in the list
    struct Note *next;
} Note;


/// Functions
void insert_note(Note *song, Note *note);
void play_song(Note *song, SDLKey *keys);


#endif // KEY_H_INCLUDED
