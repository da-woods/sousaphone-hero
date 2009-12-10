/***********************************************************************
    tune.hpp

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

#ifndef TUNE_HPP
#define TUNE_HPP

#include <string>

const char NO_NOTE_CHARACTER = '.';

const double DEFAULT_TEMPO = 60.0;

void loadTune(const std::string& filename);

// does a command (in input file e.g. tempo)
void doCommand(const std::string& command);

const std::string& tune();

std::string randomTune();

// gets the tempo in bpm
double getTempo();
void setTempo(const double tempo);

char get2ndNextNote();
char getNextNote();
char getCurrentNote();
char getLastNote();
char get2ndLastNote();

enum NotePlayed { NOT_YET_PLAYED, PLAYED_CORRECTLY, PLAYED_WRONGLY };
void setCurrentNotePlayed(NotePlayed manner);
NotePlayed currentNoteHasBeenPlayed();
NotePlayed lastNoteHasBeenPlayed();
NotePlayed last2ndNoteHasBeenPlayed();
inline NotePlayed nextNoteHasBeenPlayed() { return NOT_YET_PLAYED; }
inline NotePlayed next2ndNoteHasBeenPlayed() { return NOT_YET_PLAYED; }

void incrementTune();

// fracton of time left for a note
double noteFraction();


#endif
