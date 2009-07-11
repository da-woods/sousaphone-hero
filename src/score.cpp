#include "score.hpp"

#include <cmath>

#include "SDL/SDL.h"

#include "tune.hpp"
#include "general.hpp"

namespace score {

// score control parameters
static const signed int SCORE_FOR_PERFECT = 100;
static const signed int SCORE_FOR_BARELY_RIGHT = -10; // when a note is played at the absolute edge of the time limit
static const signed int SCORE_FOR_WRONG_NOTE = -50;
static const signed int SCORE_FOR_MISSED_NOTE = -40;
static const signed int SCORE_FOR_EXTRA_NOTE = SCORE_FOR_WRONG_NOTE;

signed int score = 0;

char& notePendingImpl()
{
   static char notePending = '.';
   return notePending;
}

bool& notePendingOnImpl()
{
   static bool notePendingOn = false;
   return notePendingOn;
}

Uint32& notePendingEndTimeImpl() // in ms relative to getTimeSinceStart()
{
   static Uint32 endTime = 0;
   return endTime;   
}

void unsetNotePending()
{
   notePendingOnImpl() = false;
}

// duration is in seconds
void setNotePending(const char note, const double duration)
{
   const bool alreadyPending = notePendingOnImpl();

   notePendingOnImpl() = true;
   notePendingImpl() = note;
   if(!alreadyPending) // don't update time
   {
      notePendingEndTimeImpl() = getTimeSinceStart() + static_cast<Uint32>(duration*1000);
   }
}

signed int getScore() { return score; }

void addToScore(signed int addition)
{
   score += addition;
}

void registerNoteAsPlayedImpl(const char note) // this function allows no grace period to get right
{
   if (currentNoteHasBeenPlayed() == NOT_YET_PLAYED)
   {
      if (note == getCurrentNote())
      {
         const double fraction = noteFraction(); // should be near 0.5 ideally
      
         const double fractionGood = std::fabs(fraction - 0.5)*2.0;
      
         const signed int pointsAwarded = static_cast<signed int>(
                                       (fractionGood * SCORE_FOR_PERFECT) +
                                      ((1.0-fractionGood) * SCORE_FOR_BARELY_RIGHT));
      
         addToScore(pointsAwarded);
   
         setCurrentNotePlayed(PLAYED_CORRECTLY);
      }
      else
      {
         addToScore(SCORE_FOR_WRONG_NOTE);
         setCurrentNotePlayed(PLAYED_WRONGLY);
      }
   }
   else
      addToScore(SCORE_FOR_EXTRA_NOTE);
}

void registerNoteAsPlayed(const char note)
{
   if (note == getCurrentNote()) // note is correct
   {
      unsetNotePending();
      registerNoteAsPlayedImpl(note);
   }
   else // give benefit of doubt for time
   {
      setNotePending(note,TIME_TO_RECTIFY*60.0/getTempo());
   }
}

void registerMissedNote()
{
   if (getCurrentNote() != NO_NOTE_CHARACTER) // ignore notes that are blank
   {
      addToScore(SCORE_FOR_MISSED_NOTE);
      setCurrentNotePlayed(PLAYED_WRONGLY);
   }
}

void doEveryLoop()
{
   if (notePendingOnImpl())
   {
      if (notePendingEndTimeImpl() < getTimeSinceStart())
      {
         registerNoteAsPlayedImpl(notePendingImpl());
         unsetNotePending();
      }
   }
}



}
