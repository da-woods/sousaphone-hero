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
