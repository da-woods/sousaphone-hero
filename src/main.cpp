/***********************************************************************
    main.cpp

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

#include "SDL/SDL.h"
#include "SDL_mixer.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>

// internal includes
#include "general.hpp"
#include "notes.hpp"
#include "keys.hpp"
#include "graphics.hpp"
#include "tune.hpp"
#include "score.hpp"

int main(int argc, char *argv[]) try
{
   setPath(argv[0]);

   class InitAndQuit
   {
      public:
         InitAndQuit()
         {
            if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
            {
               std::ostringstream error;
               error << "Failed on SDL_Init: " << SDL_GetError() << "\n";
               throw std::runtime_error(error.str());
            }
            graphics::setupGraphics();
            SDL_WM_SetCaption("Sousaphone Hero", "Sousaphone Hero");
            if(Mix_OpenAudio(AUDIO_RATE, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_BUFFERS) != 0)
            {
               SDL_Quit();
               std::ostringstream error;
               error << "Failed on Mix_OpenAudio: " << Mix_GetError() << "\n";
               throw std::runtime_error(error.str());
            }

            std::srand(time(NULL));

            Mix_ChannelFinished(doChangedNotes); // set callback function

            startTiming();
         }

         ~InitAndQuit()
         {
            graphics::cleanupGraphics();
            Mix_CloseAudio();
            SDL_Quit();
         }
   } iandq;

   // load all the notes (to load them)
   getNoteA(); getNoteB(); getNoteC(); getNoteD(); getNoteE(); getNoteF(); getNoteG();

   const bool demoMode = false;

   // load tune if argument is given
   if (argc > 1)
      loadTune(argv[1]);

   double lastNoteFraction = 0;

   int channel = -1;

   bool tuneStarted = false;

   try
   {
      while(1)
      {
         const double currentNoteFraction = noteFraction();
         if (currentNoteFraction < lastNoteFraction) // i.e. we have wrapped round a note
         {
            incrementTune();
            if (demoMode)
            {
               const char current = getCurrentNote();
               if (current != NO_NOTE_CHARACTER)
               {
                  if (channel != -1)
                     Mix_HaltChannel(channel);

                  channel = Mix_PlayChannelTimed(-1, getNote(current).get(), -1, NOTE_DURATION);
               }
            }
         }
         lastNoteFraction = currentNoteFraction;

         if ((getTimeSinceStart() > graphics::ZOOM_OUT_LENGTH*1000.0) && !tuneStarted)
         {
            startTune();
            tuneStarted = true;
         }

         //SDL_Delay(100);

         doIOLoop();
         playAppropriateNote();
         graphics::draw(currentNoteFraction);
         graphics::updateScreen();
         score::doEveryLoop();
      }
   }
   catch(QuitSignal&)
   {} // do nothing

   return 0;
} catch (std::exception& error)
{
   std::cout << error.what();
   return 1;
}
