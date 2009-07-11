#ifndef SCORE_HPP
#define SCORE_HPP

namespace score {

signed int getScore();

void addToScore(signed int addition);

// marks a note as player
// this function has a short time delay to ensure other keys are pressed
void registerNoteAsPlayed(const char note);

// this should be done while the note is still the current note
// mark it appropriately
void registerMissedNote();

// checks for the existance of notes that have passed their grace period
void doEveryLoop();

}

#endif
