# XPhone

This repository contains the firmware for the XPhone project.
This firmware runs on an STM32 microcontroller platform.

The XPhone is an automatically-controlled xylophone (technically a glockenspiel).
I selected the NUCLEO-F412ZG to control this musical instrument.

## XPhone Theory of Operation
Each metal key has a magnet attached to it. When the metal keys vibrate, the magnets produce a time-varying magnetic field that is picked up by small coils embedded in the body of the instrument. The signal from the coils is amplified and played through speakers (or recorded). Circuitry exists to convert the analog waveform from the coils into a digital waveform. A microcontroller detects this digital waveform and interprets it as a key hit. The microcontroller keeps track of which keys the user has played. When the user hits the instrument's pedal (like an electric keyboard's pedal), the microcontroller repeats what the user has just played. The microcontroller is able to play the keys as well. It controls circuitry that powers solenoids that hit the keys from underneath.

The XPhone will loop the melody infinitely (until the program is restarted or the song is cleared).
The user is able to add more layers of melody to the existing loop.
The system is continually interactive. It works to allow the user to "build up" a melody over successive iterations through the song loop.
