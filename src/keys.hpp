/***********************************************************************
    keys.hpp

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

#ifndef KEYS_HPP
#define KEYS_HPP

// sousaphone keys and fingering

#include "SDL/SDL.h"

#include <iosfwd>
#include <map>

const unsigned int NOTE_DURATION = 1000; // ms
const unsigned int NOTE_EXPIREY_DURATION = 500;

class Fingering
{
   public:
      // make from current keyboard / mouse state
      Fingering();

      bool key1() const { return key1State; }
      bool key2() const { return key2State; }
      bool key3() const { return key3State; }

      // makes from specified 3 bools
      Fingering(const bool k1, const bool k2, const bool k3);

      bool operator==(const Fingering& rhs) const
      {
         return (key1State == rhs.key1State) && (key2State == rhs.key2State) && (key3State == rhs.key3State);
      }

      // for maps
      bool operator<(const Fingering& rhs) const
      {
         if (key1State != rhs.key1State)
            return (key1State < rhs.key1State);
         else if (key2State != rhs.key2State)
            return (key2State < rhs.key2State);
         else
            return (key3State < rhs.key3State);
      }

      friend std::ostream& operator<<(std::ostream& lhs, const Fingering& rhs);
   private:
      bool key1State, key2State, key3State;
};

typedef std::map<Fingering,char> FingeringMap;
typedef std::map<char,Fingering> ReverseFingeringMap;

const FingeringMap& getFingeringMap();
const ReverseFingeringMap& getReverseFingeringMap();

// defines which keys control the valves
inline SDLKey getValve1Key() { return SDLK_a; }
inline SDLKey getValve2Key() { return SDLK_s; }
inline SDLKey getValve3Key() { return SDLK_d; }

inline SDLKey getShaderToggleKey() { return SDLK_t; }

// valves can also be controlled by mouse buttons
inline Uint8 getValve1Button() { return SDL_BUTTON_LEFT; }
inline Uint8 getValve2Button() { return SDL_BUTTON_MIDDLE; }
inline Uint8 getValve3Button() { return SDL_BUTTON_RIGHT; }

// gets the status of the various valves - true for pressed
bool getValve1Status();
bool getValve2Status();
bool getValve3Status();

// IO loop
void doIOLoop();

static const int NO_OF_LAST_NOTES = 16;

// get a pointer to an array of the last 16 notes played
// not neccessarily in order
const float* getNotePlayedTimes();

void setNotePlayedTime(float time);

// looks up the current fingering and plays the correct note
void playAppropriateNote(Fingering currentFingering=Fingering());

#endif
