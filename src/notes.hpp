/***********************************************************************
    notes.hpp

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

#include "notes_fwd.hpp"

#ifndef NOTES_HPP
#define NOTES_HPP

// std includes
#include <algorithm>
#include <string>

#include "SDL_mixer.h"

#include "wavegeneration_fwd.hpp"

class MixChunkHolder
{
   public:
      explicit MixChunkHolder(Mix_Chunk* mc);
      ~MixChunkHolder();

      MixChunkHolder(const MixChunkHolder& rhs);
      const MixChunkHolder& operator=(MixChunkHolder rhsCopy);

      void swap(MixChunkHolder& rhs)
      {
         using std::swap;
         swap(mixChunk, rhs.mixChunk);
      }

      // release ownership
      Mix_Chunk* release();

      // don't release ownership
      Mix_Chunk* const get() const { return mixChunk; }
   private:
      Mix_Chunk* mixChunk;
};

namespace std {
inline void swap(MixChunkHolder& lhs, MixChunkHolder& rhs)
{
   lhs.swap(rhs);
}
}

// gets a note from the specified character - throws std::runtime_error if the
// character is not a - g
// takes upper and lower case
const MixChunkHolder& getNote(const char noteCharacter);

// gets a mix chunk for each note
const MixChunkHolder& getNoteA();
const MixChunkHolder& getNoteB();
const MixChunkHolder& getNoteC();
const MixChunkHolder& getNoteD();
const MixChunkHolder& getNoteE();
const MixChunkHolder& getNoteF();
const MixChunkHolder& getNoteG();

// default frequencies (using A2-G3 which I believe is in tuba range)
// sharps are also provided
// from http://www.phy.mtu.edu/~suits/notefreqs.html
inline double defaultAFlat() { return 103.83; }
inline double defaultA() { return 110.00; }
inline double defaultASharp() { return 116.54; }
inline double defaultBFlat() { return defaultASharp(); }
inline double defaultB() { return 123.47; }
inline double defaultBSharp() { return 130.81; }
inline double defaultCFlat() { return defaultB(); }
inline double defaultC() { return defaultBSharp(); }
inline double defaultCSharp() { return 138.59; }
inline double defaultDFlat() { return defaultCSharp(); }
inline double defaultD() { return 146.83; }
inline double defaultDSharp() { return 155.56; }
inline double defaultEFlat() { return defaultDSharp(); }
inline double defaultE() { return 164.81; }
inline double defaultESharp() { return 174.61; }
inline double defaultFFlat() { return defaultE(); }
inline double defaultF() { return defaultESharp(); }
inline double defaultFSharp() { return 185.00; }
inline double defaultGFlat() { return defaultFSharp(); }
inline double defaultG() { return 196.00; }
inline double defaultGSharp() { return 207.65; }

// get the frequencies
double getAFreq();
double getBFreq();
double getCFreq();
double getDFreq();
double getEFreq();
double getFFreq();
double getGFreq();

// does the relevant note changes as required
// by setNote, but only when nothings playing
// ignored is to allow it to be a mixer callback
void doChangedNotes(int ignored);

// sets the notes to be a specific frequency
// it is safe to call these while notes are playing, since
// the change only takes effect when the note is stopped
void setNoteA(const double freq = defaultA());
void setNoteB(const double freq = defaultB());
void setNoteC(const double freq = defaultC());
void setNoteD(const double freq = defaultD());
void setNoteE(const double freq = defaultE());
void setNoteF(const double freq = defaultF());
void setNoteG(const double freq = defaultG());

void setNote(const char note, const double freq);

enum Pitch { FLAT, NATURAL, SHARP };

// allows quick setting of sharps and flats
void setNoteA(const Pitch pitch);
void setNoteB(const Pitch pitch);
void setNoteC(const Pitch pitch);
void setNoteD(const Pitch pitch);
void setNoteE(const Pitch pitch);
void setNoteF(const Pitch pitch);
void setNoteG(const Pitch pitch);

void setNote(const char note, const Pitch pitch); // set the note given

namespace wavegeneration {

// default timbre
const wavegeneration::Timbre& getInstrumentTimbre();
std::string instrumentTimbreFilename();

}

#endif
