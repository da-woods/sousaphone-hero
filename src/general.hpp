/***********************************************************************
    general.hpp

    Copyright 2008, 2009 David Woods

    This file is part of Sousaphone Hero.

    Sousaphone Hero is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sousaphone Hero is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sousaphone Hero.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************/

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
