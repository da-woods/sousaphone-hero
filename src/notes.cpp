/***********************************************************************
    notes.cpp

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

#include "notes.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "SDL_mixer.h"

#include "wavegeneration.hpp"

MixChunkHolder::MixChunkHolder(Mix_Chunk* mc) :
mixChunk(mc)
{}

MixChunkHolder::~MixChunkHolder()
{
   Mix_FreeChunk(mixChunk);
}

const MixChunkHolder& MixChunkHolder::operator=(MixChunkHolder rhsCopy)
{
   swap(rhsCopy);
   return *this;
}

// functions where notes are statically stored
MixChunkHolder& getNoteAImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getAFreq())));
   return mc;
}

MixChunkHolder& getNoteBImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getBFreq())));
   return mc;
}

MixChunkHolder& getNoteCImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getCFreq())));
   return mc;
}

MixChunkHolder& getNoteDImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getDFreq())));
   return mc;
}
MixChunkHolder& getNoteEImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getEFreq())));
   return mc;
}

MixChunkHolder& getNoteFImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getFFreq())));
   return mc;
}

MixChunkHolder& getNoteGImpl()
{
   using namespace wavegeneration;
   static MixChunkHolder mc(createMixChunk(getInstrumentTimbre().getWave(getGFreq())));
   return mc;
}

// bit with the frequencies are statically stored
double& getFreqAImpl()
{
   static double freq = defaultA();
   return freq;
}

double& getFreqBImpl()
{
   static double freq = defaultB();
   return freq;
}

double& getFreqCImpl()
{
   static double freq = defaultC();
   return freq;
}

double& getFreqDImpl()
{
   static double freq = defaultD();
   return freq;
}

double& getFreqEImpl()
{
   static double freq = defaultE();
   return freq;
}

double& getFreqFImpl()
{
   static double freq = defaultF();
   return freq;
}

double& getFreqGImpl()
{
   static double freq = defaultG();
   return freq;
}

namespace wavegeneration {

// function to do one of timbre loading
Timbre getInstrumentTimbreImpl()
{
   std::ifstream fin(instrumentTimbreFilename().c_str());
   if (!fin)
      std::runtime_error(std::string("Failed to load timbre file ") + instrumentTimbreFilename() + "\n");
   return Timbre(fin);
}

std::string instrumentTimbreFilename()
{
   return getPath() + "data/timbre.txt";
}

// instrument timbre is statically stored
const Timbre& getInstrumentTimbre()
{
   static Timbre timbre(getInstrumentTimbreImpl());
   return timbre;
}

}

const MixChunkHolder& getNote(const char noteCharacter)
{
   const char upperChar = toupper(noteCharacter);
   switch(upperChar)
   {
      case('A'):
         return getNoteA(); break;
      case('B'):
         return getNoteB(); break;
      case('C'):
         return getNoteC(); break;
      case('D'):
         return getNoteD(); break;
      case('E'):
         return getNoteE(); break;
      case('F'):
         return getNoteF(); break;
      case('G'):
         return getNoteG(); break;
      default:
         std::ostringstream error;
         error << "Unknown note selected: " << upperChar << "\n";
         throw std::runtime_error(error.str());
   }
}

// get note funcs
const MixChunkHolder& getNoteA()
{
   return getNoteAImpl();
}

const MixChunkHolder& getNoteB()
{
   return getNoteBImpl();
}

const MixChunkHolder& getNoteC()
{
   return getNoteCImpl();
}

const MixChunkHolder& getNoteD()
{
   return getNoteDImpl();
}

const MixChunkHolder& getNoteE()
{
   return getNoteEImpl();
}

const MixChunkHolder& getNoteF()
{
   return getNoteFImpl();
}

const MixChunkHolder& getNoteG()
{
   return getNoteGImpl();
}

// get frequency funcs
double getAFreq()
{
   return getFreqAImpl();
}

double getBFreq()
{
   return getFreqBImpl();
}

double getCFreq()
{
   return getFreqCImpl();
}

double getDFreq()
{
   return getFreqDImpl();
}

double getEFreq()
{
   return getFreqEImpl();
}

double getFFreq()
{
   return getFreqFImpl();
}

double getGFreq()
{
   return getFreqGImpl();
}

// get generally, takes 3 freqencies (flat, natural, sharp)
// returns the right one
double getFrequency(const Pitch pitch, const double flatFreq, const double naturalFreq, const double sharpFreq)
{
   switch (pitch)
   {
      case (FLAT):
         return flatFreq; break;
      case (NATURAL):
         return naturalFreq; break;
      case (SHARP):
         return sharpFreq; break;
      default:
         throw std::runtime_error("Argument was not sharp, flat or natural.\n");
   }
}


// set sharps or flats
void setNoteA(const Pitch pitch)
{
   setNoteA(getFrequency(pitch, defaultAFlat(), defaultA(), defaultASharp()));
}

// set sharps or flats
void setNoteB(const Pitch pitch)
{
   setNoteB(getFrequency(pitch, defaultBFlat(), defaultB(), defaultBSharp()));
}

// set sharps or flats
void setNoteC(const Pitch pitch)
{
   setNoteC(getFrequency(pitch, defaultCFlat(), defaultC(), defaultCSharp()));
}

// set sharps or flats
void setNoteD(const Pitch pitch)
{
   setNoteD(getFrequency(pitch, defaultDFlat(), defaultD(), defaultDSharp()));
}

// set sharps or flats
void setNoteE(const Pitch pitch)
{
   setNoteE(getFrequency(pitch, defaultEFlat(), defaultE(), defaultESharp()));
}

// set sharps or flats
void setNoteF(const Pitch pitch)
{
   setNoteF(getFrequency(pitch, defaultFFlat(), defaultF(), defaultFSharp()));
}

// set sharps or flats
void setNoteG(const Pitch pitch)
{
   setNoteG(getFrequency(pitch, defaultGFlat(), defaultG(), defaultGSharp()));
}

void setNoteAImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqAImpl() = freq;

   getNoteAImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getAFreq())));
}

void setNoteBImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqBImpl() = freq;

   getNoteBImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getBFreq())));
}

void setNoteCImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqCImpl() = freq;

   getNoteCImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getCFreq())));
}

void setNoteDImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqDImpl() = freq;

   getNoteDImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getDFreq())));
}

void setNoteEImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqEImpl() = freq;

   getNoteEImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getEFreq())));
}

void setNoteFImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqFImpl() = freq;

   getNoteFImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getFFreq())));
}

void setNoteGImpl(const double freq)
{
   using namespace wavegeneration;
   // change value of reference
   getFreqGImpl() = freq;

   getNoteGImpl() = MixChunkHolder(createMixChunk(getInstrumentTimbre().getWave(getGFreq())));
}

typedef std::vector<std::pair<void (*)(const double), double> > FuncNoteVec;

FuncNoteVec& getNoteChangeVector()
{
   static FuncNoteVec noteVecs;
   return noteVecs;
}

void addToNoteChangeVector(void (*func)(const double), const double freq)
{
   if (Mix_Playing(-1) == 0)
      func(freq); // nothing playing, can do it now
   else
      getNoteChangeVector().push_back(std::make_pair(func, freq));
}

void setNoteA(const double freq)
{
   addToNoteChangeVector(setNoteAImpl, freq);
}

void setNoteB(const double freq)
{
   addToNoteChangeVector(setNoteBImpl, freq);
}

void setNoteC(const double freq)
{
   addToNoteChangeVector(setNoteCImpl, freq);
}

void setNoteD(const double freq)
{
   addToNoteChangeVector(setNoteDImpl, freq);
}

void setNoteE(const double freq)
{
   addToNoteChangeVector(setNoteEImpl, freq);
}

void setNoteF(const double freq)
{
  addToNoteChangeVector(setNoteFImpl, freq);
}

void setNoteG(const double freq)
{
   addToNoteChangeVector(setNoteGImpl, freq);
}

void doChangedNotes(int) // ignore the input
{
   if(Mix_Playing(-1) != 0) // if anything's playing
      return; // wait until later

   FuncNoteVec& changes = getNoteChangeVector();
   const FuncNoteVec::const_iterator end = changes.end();
   for (FuncNoteVec::const_iterator i = changes.begin(); i != end; ++i)
   {
      i->first(i->second); // call the function stored with the frequency stored
   }

   changes.clear(); // all done, so clear the list
}

void setNote(const char note, const double freq)
{
   const char upperNote = toupper(note);
   switch(upperNote)
   {
      case('A'):
         setNoteA(freq);
         break;
      case('B'):
         setNoteB(freq);
         break;
      case('C'):
         setNoteC(freq);
         break;
      case('D'):
         setNoteD(freq);
         break;
      case('E'):
         setNoteE(freq);
         break;
      case('F'):
         setNoteF(freq);
         break;
      case('G'):
         setNoteG(freq);
         break;
      default:
         std::cout << "Unknown note selected: " << upperNote << "\n";
   }
}

void setNote(const char note, const Pitch pitch)
{
   const char upperNote = toupper(note);
   switch(upperNote)
   {
      case('A'):
         setNoteA(pitch);
         break;
      case('B'):
         setNoteB(pitch);
         break;
      case('C'):
         setNoteC(pitch);
         break;
      case('D'):
         setNoteD(pitch);
         break;
      case('E'):
         setNoteE(pitch);
         break;
      case('F'):
         setNoteF(pitch);
         break;
      case('G'):
         setNoteG(pitch);
         break;
      default:
         std::cout << "Unknown note selected: " << upperNote << "\n";
   }
}

/*void setNoteB(const Pitch pitch);
void setNoteC(const Pitch pitch);
void setNoteD(const Pitch pitch);
void setNoteE(const Pitch pitch);
void setNoteF(const Pitch pitch);
void setNoteG(const Pitch pitch);*/
