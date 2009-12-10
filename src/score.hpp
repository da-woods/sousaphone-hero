/***********************************************************************
    score.hpp

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
