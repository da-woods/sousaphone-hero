/***********************************************************************
    tune.cpp

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

#include "tune.hpp"

#include "SDL/SDL.h"

#include <string>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
//#include <iterator>
#include <algorithm>
#include <iostream>


#include "general.hpp"
#include "score.hpp"
#include "notes.hpp"

//const std::string tune("abcdefg.fedcba");

double tempo = DEFAULT_TEMPO;

std::string::const_iterator doIncrement(std::string::const_iterator it);

NotePlayed& lastNotePlayedImpl();

NotePlayed& last2ndNotePlayedImpl();

NotePlayed& currentNotePlayedImpl();

std::string::const_iterator& next2ndNoteIterator();

std::string::const_iterator& nextNoteIterator();

std::string::const_iterator& currentNoteIterator();

std::string::const_iterator& lastNoteIterator();

std::string::const_iterator& last2ndNoteIterator();

std::string& next2ndCommand();
std::string& nextCommand(); // stores the pending command

// does the work for the keychange
void keyChange(std::istream& input)
{
   //input.get(); // forward 1;
   while(input)
   {
      char note;
      input >> note;// = input.get();
      if(!input)
         break;
      const char nextChar = input.get();
//      input >> nextChar;// = input.get();
      if (nextChar == 'b') // flatten
      {
         setNote(note, FLAT);
         //input.get(); // forward 1 to skip space
         continue;
      }
      else if(nextChar == '#') // sharpen
      {
         setNote(note, SHARP);
         //input.get(); // forward 1 to skip space
         continue;
      }
      // now nextChar can only equal space
      else if (!std::isspace(nextChar))
      {
         std::cout << "Unrecognised character of '" << nextChar << "' in keychage.\n";
         return;
      }
      const char thirdChar = input.peek();
      if (std::isdigit(thirdChar)) // a pitch is being set
      {
         double newFreq;
         input >> newFreq;
         if(input.fail())
         {
            std::cout << "Failed to read frequency in keychange.\n";
            return;
         }
         //input.get(); // forward 1
         setNote(note,newFreq);
      }
      else // must be reseting to natural
      {
         setNote(note,NATURAL);
         continue;
      }
   }
}

void doCommand(const std::string& command)
{
   std::istringstream input(command);
   std::string mainCommand;

   input >> mainCommand;
   if (mainCommand == "tempo") // set new tempo
   {
      double tempo;
      input >> tempo;
      if (!input)
      {
         std::cout << "Failure parsing new tempo in command " << command << ". Ignoring....\n";
      }
      setTempo(tempo);
   }
   else if (mainCommand == "tempomult") // multiply the tempo
   {
      double factor;
      input >> factor;
      if (!input)
      {
         std::cout << "Failure parsing tempo multiplication in command " << command << ". Ignoring....\n";
      }
      setTempo(factor * getTempo());
   }
   else if (mainCommand == "key") // change the key
   {
      keyChange(input);
   }
   else
   {
      std::cout << "Ignoring unrecognised command: " << command << "\n";
   }
}

double getTempo()
{
   return tempo;
}

void setTempo(const double t)
{
   tempo = t;
   // also reset tune timer
   startTune();
}

std::string& tuneImpl()
{
   static std::string tune;
   return tune;
}

const std::string& tune()
{
   std::string& tune = tuneImpl();
   if (tune.empty())
   {
      tune = randomTune();// randomly generate tune
   }

   return tune;
}

void loadTune(const std::string& filename)
{
   std::string& tune = tuneImpl();
   tune.clear();

   std::ifstream fin(filename.c_str());

   if(!fin)
   {
      std::ostringstream error;
      error << "Failed to load tune in " << filename << "\n";
      throw std::runtime_error(error.str());
   }

   //tune.append(std::istream_iterator<char>(fin), std::istream_iterator<char>());
   std::ostringstream ss;
   ss << fin.rdbuf();
   tuneImpl() = ss.str();

   // kill all the iterators
   lastNotePlayedImpl() = NOT_YET_PLAYED;
   last2ndNotePlayedImpl() = NOT_YET_PLAYED;
   currentNotePlayedImpl() = NOT_YET_PLAYED;
   next2ndNoteIterator() = doIncrement(tune.end()-1);
   nextNoteIterator() = tune.end();
   currentNoteIterator() = tune.end();
   lastNoteIterator() = tune.end();
   last2ndNoteIterator() = tune.end();
   next2ndCommand() = std::string();
   nextCommand() = std::string();
}

std::string randomTune()
{
   static const unsigned int MAX_LENGTH = 500;
   static const unsigned int MIN_LENGTH = 2;

   const unsigned int length = MIN_LENGTH +
         static_cast<unsigned int>((MAX_LENGTH - MIN_LENGTH) * static_cast<double>(std::rand()) / RAND_MAX);

   std::string tune;
   tune.reserve(length);

   for (unsigned int i = 0; i < length; ++i)
   {
      const double val = static_cast<double>(std::rand())/RAND_MAX;

      if (val < (1.0/7.0))
         tune += 'a';
      else if (val < (2.0 / 7.0))
         tune += 'b';
      else if (val < (3.0 / 7.0))
         tune += 'c';
      else if (val < (4.0 / 7.0))
         tune += 'd';
      else if (val < (5.0 / 7.0))
         tune += 'e';
      else if (val < (6.0 / 7.0))
         tune += 'f';
      else
         tune += 'g';
   }

   return tune;
}

NotePlayed& lastNotePlayedImpl()
{
   static NotePlayed played = NOT_YET_PLAYED;
   return played;
}

NotePlayed& last2ndNotePlayedImpl()
{
   static NotePlayed played = NOT_YET_PLAYED;
   return played;
}

NotePlayed& currentNotePlayedImpl()
{
   static NotePlayed played = NOT_YET_PLAYED;
   return played;
}

std::string::const_iterator& next2ndNoteIterator()
{
   static std::string::const_iterator i = doIncrement(tune().end()-1);
   return i;
}

std::string::const_iterator& nextNoteIterator()
{
   static std::string::const_iterator i = tune().end();
   return i;
}

std::string::const_iterator& currentNoteIterator()
{
   static std::string::const_iterator i = tune().end();
   return i;
}

std::string::const_iterator& lastNoteIterator()
{
   static std::string::const_iterator i = tune().end();
   return i;
}

std::string::const_iterator& last2ndNoteIterator()
{
   static std::string::const_iterator i = tune().end();
   return i;
}

std::string& next2ndCommand()
{
   static std::string command;
   return command;
}

std::string& nextCommand()
{
   static std::string command;
   return command;
}


std::string::const_iterator tuneBeginIterator()
{
   return tune().begin();
}

std::string::const_iterator tuneEndIterator()
{
   return tune().end();
}

std::string::const_iterator doIncrement(std::string::const_iterator it)
{
   do
   {
      ++it;

      if (it == tuneEndIterator())
         it = tuneBeginIterator();
      if (*it == '<')
      {  // find closing brace
         std::string::const_iterator i = std::find(it, tuneEndIterator(), '>');
         if (i == tuneEndIterator())
         {
            throw std::runtime_error("Opening brace without closing brace in tune");
         }
         const std::string command(it+1,i);
         next2ndCommand() = command;
         //doCommand(command);
         it = i;
      }
   } while(std::isspace(*it) || *it == '>');
   return it;
}

char getNoteFromIterator(const std::string::const_iterator& i)
{
   if (i == tuneEndIterator())
      return '.';
   else
      return *i;
}

char get2ndNextNote()
{
   return *next2ndNoteIterator();
}

char getNextNote()
{
   return getNoteFromIterator(nextNoteIterator());
}

char getCurrentNote()
{
   return getNoteFromIterator(currentNoteIterator());
}

char getLastNote()
{
   return getNoteFromIterator(lastNoteIterator());
}

char get2ndLastNote()
{
   return getNoteFromIterator(last2ndNoteIterator());
}

void incrementTune()
{
   last2ndNotePlayedImpl() = lastNoteHasBeenPlayed();
   if(currentNoteHasBeenPlayed() == NOT_YET_PLAYED)
   {
      score::registerMissedNote();
   }
   lastNotePlayedImpl() = currentNoteHasBeenPlayed();

   setCurrentNotePlayed(NOT_YET_PLAYED);

   std::string::const_iterator& next2nd = next2ndNoteIterator();

   last2ndNoteIterator() = lastNoteIterator();
   lastNoteIterator() = currentNoteIterator();
   currentNoteIterator() = nextNoteIterator();
   nextNoteIterator() = next2nd;
   if (!nextCommand().empty())
      doCommand(nextCommand()); // execute pending command
   nextCommand() = next2ndCommand();
   next2ndCommand() = std::string();

   next2nd = doIncrement(next2nd);

   //std::cout << *last2ndNoteIterator() << *lastNoteIterator() << *currentNoteIterator() << *nextNoteIterator() << *next2ndNoteIterator() << "\n";
}

double noteFraction()
{
   const double scrollRate = getTempo()/60.0; // notes per second

   const Uint32 time = getTimeSinceTuneStart(); // in millisecs
   const double timeInSecs = static_cast<double>(time)/1000;

   const double notesTraversed = timeInSecs * scrollRate;
   return std::fmod(notesTraversed,1.0);
}

void setCurrentNotePlayed(NotePlayed manner)
{
   currentNotePlayedImpl() = manner;
}

NotePlayed currentNoteHasBeenPlayed()
{
   return currentNotePlayedImpl();
}

NotePlayed lastNoteHasBeenPlayed()
{
   return lastNotePlayedImpl();
}

NotePlayed last2ndNoteHasBeenPlayed()
{
   return last2ndNotePlayedImpl();
}

