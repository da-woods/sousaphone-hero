#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "SDL/SDL.h"

#include "tune.hpp"

namespace graphics {

extern SDL_Surface* screen;

// time taken in the initial zoom out
const double ZOOM_OUT_LENGTH = 2.0; // seconds

const unsigned short X_RESOLUTION = 800;
const unsigned short Y_RESOLUTION = 600;
const unsigned short COLOUR_DEPTH = 24; // bits

void setupGraphics();

void cleanupGraphics();

// use note fraction passed to it
void draw(const double noteFraction); // draws all the components but doesn't flip screen

void drawBackground();

void drawMan();

enum AnchorPosition { BOTTOM_LEFT, BOTTOM_RIGHT, TOP_LEFT, TOP_RIGHT };

void writeNumber(const signed int number, const double x, const double y, 
   const AnchorPosition pos, const double size);

// renders the fingering passing along the top of the screen
void drawTune(const double noteFraction, const bool showGuide = true);

// draws the fingering for a note, centred on position given
// remembering that coordinates go from 1 to -1
// size is relative
void drawFingering(const char note, const double x, const double y, const double size, const NotePlayed played);

// uses vertex shaders to draw insects flitting everywhere
void drawInsects();

// doesn't do drawing, just flips the screen!
void updateScreen();

}

#endif
