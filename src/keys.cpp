#include "keys.hpp"

#include "notes.hpp"
#include "general.hpp"
#include "score.hpp"
#include "tune.hpp"

#include <stdexcept>
#include <map>
#include <sstream>

#include "SDL/SDL.h"
#include "SDL_mixer.h"



Fingering::Fingering() :
key1State(getValve1Status()), key2State(getValve2Status()), key3State(getValve3Status())
{}

Fingering::Fingering(const bool k1, const bool k2, const bool k3) :
key1State(k1), key2State(k2), key3State(k3)
{}

std::ostream& operator<<(std::ostream& lhs, const Fingering& rhs)
{
   lhs << rhs.key1State << " " << rhs.key2State << " " << rhs.key3State << " ";
   return lhs;
}

// one of generation of map between default fingering and notes
FingeringMap generateFingeringMap()
{
   FingeringMap theMap;
   theMap[Fingering(true, false, false)] = 'a';
   theMap[Fingering(false, true, false)] = 'b';
   theMap[Fingering(false, false, true)] = 'c';
   theMap[Fingering(true, true, false)] = 'd';
   theMap[Fingering(true, false, true)] = 'e';
   theMap[Fingering(false, true, true)] = 'f';
   theMap[Fingering(true, true, true)] = 'g';

   return theMap;
}

ReverseFingeringMap generateReverseFingeringMap()
{
   ReverseFingeringMap theMap;
   const FingeringMap& fmap = getFingeringMap();

   const FingeringMap::const_iterator end = fmap.end();

   for(FingeringMap::const_iterator i = fmap.begin(); i != end; ++i)
   {
      theMap[i->second] = i->first;
   }

   return theMap;
}

const FingeringMap& getFingeringMap()
{
   static FingeringMap fmap(generateFingeringMap());
   return fmap;
}

const ReverseFingeringMap& getReverseFingeringMap()
{
   static ReverseFingeringMap rfmap(generateReverseFingeringMap());
   return rfmap;
}

// true if pressed
bool& key1State()
{
   static bool state = false;
   return state;
}

bool& key2State()
{
   static bool state = false;
   return state;
}

bool& key3State()
{
   static bool state = false;
   return state;
}

bool& key1DelayedState()
{
   static bool state = false;
   return state;
}

bool& key2DelayedState()
{
   static bool state = false;
   return state;
}

bool& key3DelayedState()
{
   static bool state = false;
   return state;
}

// time the keypresses presses are delayed until
Uint32& pressesDelayedToTime()
{
   static Uint32 time = 0;
   return time;
}

void setDelayTime(const double delayInSeconds)
{
   pressesDelayedToTime() = getTimeSinceStart() + static_cast<Uint32>(delayInSeconds * 1000.0);
}


bool& key1DelayedPending()
{
   static bool pending = false;
   return pending;
}

bool& key2DelayedPending()
{
   static bool pending = false;
   return pending;
}

bool& key3DelayedPending()
{
   static bool pending = false;
   return pending;
}

void unsetPending()
{
   key1DelayedPending() = false;
   key2DelayedPending() = false;
   key3DelayedPending() = false;
}

void updateThenUnsetPending()
{
   if (key1DelayedPending())
      key1State() = key1DelayedState();
   if (key2DelayedPending())
      key2State() = key2DelayedState();
   if (key3DelayedPending())
      key3State() = key3DelayedState();

   unsetPending();
}



// checks for those keys we haven't yet pressed and implements them
void doDelayedPresses()
{
   if(getTimeSinceStart() > pressesDelayedToTime())
   {
      updateThenUnsetPending();
   }
}

// check to see if the note is one we should register and play instantly (i.e. all keys released)
// or the correct note
void checkDelayedPressKeys()
{
   const bool key1 = key1DelayedPending() ? key1DelayedState() : key1State();
   const bool key2 = key2DelayedPending()? key2DelayedState() : key2State();
   const bool key3 = key3DelayedPending() ? key3DelayedState() : key3State();

   // if we're playing nothing, then set that now!
   if (!key1 && !key2 && !key3)
   {
      updateThenUnsetPending();
   }
   else
   {
      const Fingering hypothetical(key1,key2,key3);
      const FingeringMap& fmap = getFingeringMap();
      const FingeringMap::const_iterator i = fmap.find(hypothetical);

      if (i != fmap.end()) // ignore it if not found, I don't care
      {
         const char notePlayed = i->second;
         if (notePlayed == getCurrentNote())
            updateThenUnsetPending();
      }
      else
      {
         //std::cerr << "Fingering " << hypothetical << "not found! Ignoring....\n";
      }
   }
}

void delayedKey1Press(bool pressed)
{
   const bool alreadyPending = key1DelayedPending() || key2DelayedPending() || key3DelayedPending();

   key1DelayedState() = pressed;
   key1DelayedPending() = true;

   // add time
   if (!alreadyPending)
   {
      setDelayTime(TIME_TO_RECTIFY * getTempo()/60.0);
   }

   checkDelayedPressKeys();
}

void delayedKey2Press(bool pressed)
{
   const bool alreadyPending = key1DelayedPending() || key2DelayedPending() || key3DelayedPending();

   key2DelayedState() = pressed;
   key2DelayedPending() = true;

   // add time
   if (!alreadyPending)
   {
      setDelayTime(TIME_TO_RECTIFY * getTempo()/60.0);
   }

   checkDelayedPressKeys();
}

void delayedKey3Press(bool pressed)
{
   const bool alreadyPending = key1DelayedPending() || key2DelayedPending() || key3DelayedPending();

   key3DelayedState() = pressed;
   key3DelayedPending() = true;

   // add time
   if (!alreadyPending)
   {
      setDelayTime(TIME_TO_RECTIFY * getTempo()/60.0);
   }

   checkDelayedPressKeys();
}

void modifyKey1State(bool pressed)
{
   if (pressed)
   {
      updateThenUnsetPending();
      key1State() = pressed;
   }
   else
   {
      // if the key has been released, it is probably not an actual 
      // attempt at a note, so we should wait to see if it's intended before
      // playing it
      delayedKey1Press(pressed);
   }
}

void modifyKey2State(bool pressed)
{
   if (pressed)
   {
      updateThenUnsetPending();
      key2State() = pressed;
   }
   else
   {
      delayedKey2Press(pressed);
   }
}

void modifyKey3State(bool pressed)
{
   if (pressed)
   {
      updateThenUnsetPending();
      key3State() = pressed;
   }
   else
   {
      delayedKey3Press(pressed);
   }
}

bool getValve1Status()
{
   return key1State();
}

bool getValve2Status()
{
   return key2State();
}

bool getValve3Status()
{
   return key3State();
}

float* getNotePlayedTimesImpl()
{
   static const float NVE=-10000.0; // large negative number to start with
   static float times[NO_OF_LAST_NOTES];
   static class FillOnce
   {
      public:
         FillOnce(float* times, float value)
         {
            for (int i =0; i<NO_OF_LAST_NOTES; ++i)
               times[i] = value;
         }
   } fileOnce(times,NVE);

   return times;
}

const float* getNotePlayedTimes()
{
   return getNotePlayedTimesImpl();
}

void setNotePlayedTime(float time)
{
   static int currentPos = 0;
   getNotePlayedTimesImpl()[currentPos] = time;
   currentPos++;
   if (currentPos >= NO_OF_LAST_NOTES)
      currentPos = 0;
}


void doIOLoop()
{
   // checks for presses we haven't yet dealt with, and
   // implements them
   doDelayedPresses();

   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
         case(SDL_KEYDOWN):
         case(SDL_KEYUP): // deal with keyboard events identically
         {
            const bool keydown = (event.type == SDL_KEYDOWN);
            if(event.key.keysym.sym == getValve1Key())
            {
               modifyKey1State(keydown);
            }
            else if (event.key.keysym.sym == getValve2Key())
            {
               modifyKey2State(keydown);
            }
            else if (event.key.keysym.sym == getValve3Key())
            {
               modifyKey3State(keydown);
            } // else irrelevent
            break;
         }
         case(SDL_MOUSEBUTTONDOWN):
         case(SDL_MOUSEBUTTONUP):
         {
            const bool keydown = (event.type == SDL_MOUSEBUTTONDOWN);
            if(event.button.button == getValve1Button())
            {
               modifyKey1State(keydown);
            }
            else if (event.button.button == getValve2Button())
            {
               modifyKey2State(keydown);
            }
            else if (event.button.button == getValve3Button())
            {
               modifyKey3State(keydown);
            } // else irrelevent
            break;
         }
         case(SDL_QUIT):
            throw QuitSignal(); break;
         default:
            ;// do nothing
      }
   }
}

void playAppropriateNote(Fingering currentFingering)
{
   static Fingering lastFingering(false, false, false);

   if (lastFingering == currentFingering) // nothings changed
   {
      return;
   }
   lastFingering = currentFingering; // reassign to update
   setNotePlayedTime(static_cast<float>(getTimeSinceStart())/1000.0);

   static int channel = -1;
   static bool noteExpired = false;
   if ( (currentFingering == Fingering(false,false,false)) && (!noteExpired) )
   {
      Mix_ExpireChannel(channel,NOTE_EXPIREY_DURATION); // stop playing current channel
      noteExpired = true;
      return;
   }

   //static const FingeringMap fingeringMap(generateFingeringMap());
   const FingeringMap& fingeringMap = getFingeringMap();

   FingeringMap::const_iterator i = fingeringMap.find(currentFingering);
   if (i == fingeringMap.end())
   {
      std::ostringstream error;
      error << "Fingering not found in fingering map! " 
            << "This is a programming mistake since the map must have been generated incompletely. "
            << "Fingering given was " << currentFingering << "\n";
      throw std::logic_error(error.str());
   }


   if (channel != -1)
   {
      Mix_HaltChannel(channel); // stop playing current channel
      noteExpired = true;
   }

   channel = Mix_PlayChannelTimed(-1, getNote(i->second).get(), -1, NOTE_DURATION);
   noteExpired = false;

   score::registerNoteAsPlayed(i->second);
}
