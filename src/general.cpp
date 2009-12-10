/***********************************************************************
    general.cpp

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


#include "general.hpp"

#include <iostream>

Uint32& getTimeOffset()
{
   static Uint32 timeOffset = 0;
   return timeOffset;
}

void startTiming()
{
   getTimeOffset() = SDL_GetTicks();  
}

Uint32 getTimeSinceStart()
{
   Uint32 time = SDL_GetTicks();
   return time - getTimeOffset();   
}

Uint32& getTuneTimeOffset()
{
   static Uint32 timeOffset = 0;
   return timeOffset;
}

bool& tuneStarted()
{
   static bool started = false;
   return started;
}

void startTune()
{
   getTuneTimeOffset() = SDL_GetTicks();
   tuneStarted() = true;
}

Uint32 getTimeSinceTuneStart()
{
   if (tuneStarted())
   {
      Uint32 time = SDL_GetTicks();
      return time - getTuneTimeOffset();
   }
   else
      return 0;
}

std::string& path()
{
   static std::string path;
   return path;
}

void setPath(const std::string& programPath)
{
   static const std::string SLASHES = "/\\"; // withdows and linux possibilites

   // find last slash
   const std::string::size_type index = programPath.find_last_of(SLASHES);

   if (index == std::string::npos)
   {
      std::cout << "Character " << SLASHES << " not found in path " << programPath << "\n";
      return;
   }

   // path without program name
   const std::string newPath(programPath,0,index+1);
   //std::cout << newPath << "\n";
   path() = newPath;
}

const std::string& getPath()
{
   return path();
}
