/***********************************************************************
    graphics.hpp

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

void setupGraphics(bool loadShaders = true);

void cleanupGraphics();

// use note fraction passed to it
void draw(const double noteFraction); // draws all the components but doesn't flip screen

void drawBackground();

void drawMan();

void toggleShaders();

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
