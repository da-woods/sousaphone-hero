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
