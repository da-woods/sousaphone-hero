#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <string>

#include "SDL/SDL.h"

const int AUDIO_RATE = 22050; // hz
const Uint16 AUDIO_FORMAT = AUDIO_S16SYS; // signed 16 bit with system endianness
const int AUDIO_CHANNELS = 2;
const int AUDIO_BUFFERS = 4096;

typedef Sint16 SoundDataType;

// exception to allow quitting easily
class QuitSignal
{};

void startTiming();
Uint32 getTimeSinceStart(); // gets the time since startTiming was called (or since the program was initialised if start timing wasn't called)

void startTune();
Uint32 getTimeSinceTuneStart(); // a separate timer for the music than the animation.

const double TIME_TO_RECTIFY = 0.1; // time (as a fraction of tempo) to set a note right (allows for non-instant key action)

void setPath(const std::string& programPath);

const std::string& getPath();

#endif
