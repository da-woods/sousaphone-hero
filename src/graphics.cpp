/***********************************************************************
    graphics.cpp

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

#include "graphics.hpp"

#include <stdexcept>
#include <sstream>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>

#include "SDL/SDL_opengl.h"

#include "keys.hpp"
#include "tune.hpp"
#include "general.hpp"
#include "score.hpp"

namespace graphics {

// the screen
SDL_Surface* screen = 0;

bool shadersActive = true;

const double DRAW_DEPTH = -0.5;

GLuint crowdTexture = 0, 
       playerTexture = 0,
       valveTexture = 0,
       numbersTexture = 0,
       minusTexture = 0,
       tickCrossTexture = 0,
       beeTexture = 0,
       butterflyTexture = 0;

// shaders (remain at zero if unavailable)
GLuint insectShader = 0,
       soundWaveShader = 0;

// gl function pointers
PFNGLCREATESHADERPROC fpglCreateShader = 0;
PFNGLCREATEPROGRAMPROC fpglCreateProgram = 0;
PFNGLSHADERSOURCEPROC fpglShaderSource = 0;
PFNGLCOMPILESHADERPROC fpglCompileShader = 0;
PFNGLGETSHADERIVPROC fpglGetShaderiv = 0;
PFNGLGETSHADERINFOLOGPROC fpglGetShaderInfoLog = 0;
PFNGLATTACHSHADERPROC fpglAttachShader = 0;
PFNGLLINKPROGRAMPROC fpglLinkProgram = 0;
PFNGLGETPROGRAMIVPROC fpglGetProgramiv = 0;
PFNGLGETPROGRAMINFOLOGPROC fpglGetProgramInfoLog = 0;
PFNGLUSEPROGRAMPROC fpglUseProgram = 0;
PFNGLGETUNIFORMLOCATIONPROC fpglGetUniformLocation = 0;
PFNGLUNIFORM1FVPROC fpglUniform1fv = 0;
PFNGLUNIFORM4FVPROC fpglUniform4fv = 0;
PFNGLUNIFORM1FPROC fpglUniform1f = 0;
PFNGLUNIFORM2FPROC fpglUniform2f = 0;
PFNGLUNIFORM1IPROC fpglUniform1i = 0;
PFNGLVALIDATEPROGRAMPROC fpglValidateProgram = 0;
PFNGLDELETESHADERPROC fpglDeleteShader = 0;
PFNGLDELETEPROGRAMPROC fpglDeleteProgram = 0;

// relating to points

bool shadersViable = false; // start true, make false
bool pointSpritesEnabled = false;



std::vector<GLuint>& shaderModulesToBeDeleted()
{
   static std::vector<GLuint> modules;
   return modules;
}

// just does SDL but with output if failed
void* getProcAddress(const std::string& funcName, const std::string& alternateName = "",const bool silent=false)
{
   void* proc = SDL_GL_GetProcAddress(funcName.c_str());
   if (!proc)
   {
      // try the ARB varient
      std::string funcNameARB = funcName + "ARB";
      proc = SDL_GL_GetProcAddress(funcNameARB.c_str());
      if (!proc)
      {
         if (!silent)
            std::cout << "Failed to get function " << funcName << ".\n";
         if (!alternateName.empty())
         {
            if(!silent)
               std::cout << "... trying alternate name \"" << alternateName << "\".\n";
            proc = getProcAddress(alternateName, "", silent);
         }
      }
   }
   return proc;
}

void getGLFunctions()
{
   fpglCreateShader = (PFNGLCREATESHADERPROC)(getProcAddress("glCreateShader","glCreateShaderObject"));
   //fpglCreateShaderObject = reinterpret_cast<PFNGLCREATESHADERPROC>(getProcAddress("glCreateShaderObjectARB",true));
   fpglCreateProgram = (PFNGLCREATEPROGRAMPROC)(getProcAddress("glCreateProgram"));
   fpglShaderSource = (PFNGLSHADERSOURCEPROC)(getProcAddress("glShaderSource"));
   fpglCompileShader = (PFNGLCOMPILESHADERPROC)(getProcAddress("glCompileShader"));
   fpglGetShaderiv = (PFNGLGETSHADERIVPROC)(getProcAddress("glGetShaderiv"));
   fpglGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)(getProcAddress("glGetShaderInfoLog"));
   fpglAttachShader = (PFNGLATTACHSHADERPROC)(getProcAddress("glAttachShader"));
   fpglLinkProgram = (PFNGLLINKPROGRAMPROC)(getProcAddress("glLinkProgram"));
   fpglGetProgramiv = (PFNGLGETPROGRAMIVPROC)(getProcAddress("glGetProgramiv"));
   fpglGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)(getProcAddress("glGetProgramInfoLog"));
   fpglUseProgram = (PFNGLUSEPROGRAMPROC)(getProcAddress("glUseProgram"));
   fpglGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)(getProcAddress("glGetUniformLocation"));
   fpglUniform1fv = (PFNGLUNIFORM1FVPROC)(getProcAddress("glUniform1fv"));
   fpglUniform4fv = (PFNGLUNIFORM4FVPROC)(getProcAddress("glUniform4fv"));
   fpglUniform1f = (PFNGLUNIFORM1FPROC)(getProcAddress("glUniform1f"));
   fpglUniform2f = (PFNGLUNIFORM2FPROC)(getProcAddress("glUniform2f"));
   fpglUniform1i = (PFNGLUNIFORM1IPROC)(getProcAddress("glUniform1i"));
   fpglValidateProgram = (PFNGLVALIDATEPROGRAMPROC)(getProcAddress("glValidateProgram"));
   fpglDeleteShader = (PFNGLDELETESHADERPROC)(getProcAddress("glDeleteShader"));
   fpglDeleteProgram = (PFNGLDELETEPROGRAMPROC)(getProcAddress("glDeleteProgram"));

   std::ostringstream versionString;
   versionString << glGetString(GL_VERSION);
   const bool version2 = (atof(versionString.str().c_str()) >= 2.0);

   std::ostringstream extensions;
   extensions << glGetString(GL_EXTENSIONS);
   //std::cout << glGetString(GL_EXTENSIONS);
   const bool shaderObjectsExtension = 
            (extensions.str().find("GL_ARB_shader_objects") != std::string::npos)
                                          && (sizeof(GLuint) == sizeof(GLhandleARB)); //  this second bit is needed for us to use
                                                      // the function pointers unmodified!
   //std::cout << "sizeof(GLuint) == sizeof(GLhandleARB): " << (sizeof(GLuint) == sizeof(GLhandleARB)) << "\n";

   shadersViable = fpglCreateShader && fpglCreateProgram && fpglShaderSource && fpglCompileShader && fpglGetShaderiv &&
                        fpglGetShaderInfoLog && fpglAttachShader && fpglLinkProgram && fpglGetProgramiv && fpglGetProgramInfoLog
                        && fpglUseProgram && fpglGetUniformLocation && fpglUniform1fv && fpglUniform4fv 
                        && fpglUniform1f && fpglUniform2f && fpglUniform1i && fpglValidateProgram && fpglDeleteShader && fpglDeleteProgram
                        && (version2 || shaderObjectsExtension);

   const bool pointSpritesExtension = (extensions.str().find("GL_ARB_point_sprite") != std::string::npos);

   pointSpritesEnabled = (version2 || pointSpritesExtension);
}


void makeWhiteTransparent(SDL_Surface*& surface)
{
   // we may need to make a new surface if the format is wrong
   if (surface->format->BytesPerPixel < 4)
   {
      SDL_Surface* newSurface = SDL_DisplayFormatAlpha(surface);
      if (newSurface == 0)
      {
         std::ostringstream error;
         error << "Problem converting surface to an alpha format. " << SDL_GetError() << "\n";
         throw std::runtime_error(error.str());
      }
      std::swap(surface,newSurface);
      SDL_FreeSurface(newSurface);
   }

   SDL_LockSurface(surface);
   const Uint32 whiteMask = surface->format->Rmask | surface->format->Gmask | surface->format->Bmask;
   for (unsigned long y=0; y < surface->h; ++y)
   {
      const unsigned long offset = (surface->pitch*y)/surface->format->BytesPerPixel;
      for (unsigned long x=0; x < surface->w; ++x)
      {
         Uint32* pixel = reinterpret_cast<Uint32*>(surface->pixels) + offset + x;
  
         if((*pixel & whiteMask) == whiteMask) // if the pixel is white
         {
            *pixel = whiteMask;
         }
         //else
         //   *pixel = *pixel - (surface->format->Amask & 0x88888888);
      }
   }
   SDL_UnlockSurface(surface);
}

// does all the texture initialisation, from binding the texture to loading the
// image. whiteToTransparent determines whether to replace white with alpha
void loadATexture(const std::string& filename, GLuint& textureName, bool loadMipmapped, bool whiteToTransparent=false)
{
   SDL_Surface *surface = SDL_LoadBMP(filename.c_str());
   if (!surface)
   {
      std::ostringstream error;
      error << "Failed to load texture " << filename << " due to " << SDL_GetError() << "\n";
      throw std::runtime_error(error.str());
   }

   // class to automatically free surface
   class CleanupOnQuit
   {
      public:
         CleanupOnQuit(SDL_Surface*& s) :
         surf(s)
         {}

         ~CleanupOnQuit()
         {
           if (surf)
            SDL_FreeSurface(surf);
         }
      private:
         SDL_Surface*& surf;
   } onQuit(surface);


   if (whiteToTransparent)
      makeWhiteTransparent(surface);

   const GLint noOfColours = surface->format->BytesPerPixel;

   if ((noOfColours != 4) && (noOfColours != 3))
   {
      std::ostringstream error;
      error << "Image " << filename << " is not truecolour. Instead it has " << noOfColours << " colours.\n";
      throw std::runtime_error(error.str());
   }
   const GLenum texFormat = noOfColours == 4?
      (surface->format->Rmask == 0x000000ff ? GL_RGBA : GL_BGRA) :
      (surface->format->Rmask == 0x000000ff ? GL_RGB : GL_BGR);

   // generate textures
   glGenTextures(1, &textureName);
   glBindTexture(GL_TEXTURE_2D, textureName);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   // load the image
   if (loadMipmapped)
   {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
      gluBuild2DMipmaps(GL_TEXTURE_2D, noOfColours, surface->w, surface->h, texFormat, GL_UNSIGNED_BYTE, surface->pixels);
   }
   else
   {
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexImage2D( GL_TEXTURE_2D, 0, noOfColours, surface->w, surface->h, 0,
                      texFormat, GL_UNSIGNED_BYTE, surface->pixels );
   }
}

void loadTextures()
{
   loadATexture(getPath() + "data/crowd.bmp", crowdTexture, false);
   loadATexture(getPath() + "data/player.bmp", playerTexture, false, true);
   loadATexture(getPath() + "data/valve.bmp", valveTexture, true, true);
   loadATexture(getPath() + "data/numbers.bmp", numbersTexture, false, true);
   loadATexture(getPath() + "data/minus.bmp", minusTexture, false,true);
   loadATexture(getPath() + "data/tickcross.bmp", tickCrossTexture, false, true);
   loadATexture(getPath() + "data/bee.bmp", beeTexture, false, true);
   loadATexture(getPath() + "data/butterfly.bmp", butterflyTexture, false, true);
   // I'm not overly worried about freeing them
}

GLuint loadShaderModule(const std::string& filename, const GLenum type, const bool outputDebugInfo = false)
{
   if (!shadersViable)
      return 0;

   const GLuint moduleName = fpglCreateShader(type);

   shaderModulesToBeDeleted().push_back(moduleName);

   std::ifstream fin(filename.c_str());
   if (!fin)
   {
      std::ostringstream error;
      error << "Failed to open shader in filename " << filename << ".\n";
      throw std::runtime_error(error.str());
   }
   std::ostringstream srcStream;
   srcStream << fin.rdbuf(); // copy data

   const std::string shaderSrc = srcStream.str();

   const GLchar* asCstring = shaderSrc.c_str();

   fpglShaderSource(moduleName, 1, &asCstring, NULL);
   fpglCompileShader(moduleName);
   // test if compilation worked
   GLint success;
   fpglGetShaderiv(moduleName, GL_COMPILE_STATUS, &success);
   if (!success || outputDebugInfo)
   {
      GLint logLength;
      fpglGetShaderiv(moduleName, GL_INFO_LOG_LENGTH, &logLength);
      std::vector<char> log(logLength); // make vector to store it
      GLsizei ignore;
      fpglGetShaderInfoLog(moduleName, log.size(), &ignore, &log[0]);

      std::string logAsString(log.begin(), log.end());
      if (!success)
      {
         std::ostringstream error;
         error << "Shader compilation of " << filename << " failed with error:\n" << logAsString << "\n";
         throw std::runtime_error(error.str());
      }
      else
         std::cout << "Debug info for shader " << filename << ":\n" << logAsString << "\n"; // just write
   }

   return moduleName;
}

GLuint makeShaderProgram(const std::vector<GLuint>& modules, const bool outputDebugInfo = false)
{
   if (!shadersViable)
      return 0;

   const GLuint programName = fpglCreateProgram();
   const std::vector<GLuint>::const_iterator end = modules.end();
   for (std::vector<GLuint>::const_iterator i = modules.begin(); i != end; ++i)
   {
      fpglAttachShader(programName, *i);
   }

   fpglLinkProgram(programName);

   GLint success;
   fpglGetProgramiv(programName, GL_LINK_STATUS, &success);
   if(!success || outputDebugInfo)
   {
      GLint logLength;
      fpglGetProgramiv(programName, GL_INFO_LOG_LENGTH, &logLength);
      std::vector<char> log(logLength); // make vector to store it
      GLsizei ignore;
      fpglGetProgramInfoLog(programName, log.size(), &ignore, &log[0]);

      std::string logAsString(log.begin(), log.end());
      if (!success)
      {
         std::ostringstream error;
         error << "Shader linking failed with error:\n" << logAsString << "\n";
         throw std::runtime_error(error.str());
      }
      else
         std::cout << "Debug info for linking shader:\n" << logAsString << "\n";
   }

   fpglValidateProgram(programName);
   GLint validated;
   fpglGetProgramiv(programName, GL_VALIDATE_STATUS, &validated);
   if(!success || outputDebugInfo)
   {
      GLint logLength;
      fpglGetProgramiv(programName, GL_INFO_LOG_LENGTH, &logLength);
      std::vector<char> log(logLength); // make vector to store it
      GLsizei ignore;
      fpglGetProgramInfoLog(programName, log.size(), &ignore, &log[0]);

      std::string logAsString(log.begin(), log.end());
      if (!validated)
      {
         std::ostringstream error;
         error << "Shader validation error:\n" << logAsString << "\n";
         throw std::runtime_error(error.str());
      }
      else
         std::cout << "Vaidation info for shader:\n" << logAsString << "\n";
   }

   return programName;
}

void loadShaders()
{
   static const bool DEBUG_INFO = true;;
   std::vector<GLuint> insectModules;
   const GLuint insectVertModule = loadShaderModule(getPath() + "data/shaders/insectshader.vert", GL_VERTEX_SHADER, DEBUG_INFO);
   insectModules.push_back(insectVertModule);
   const GLuint insectFragModule = loadShaderModule(getPath() + "data/shaders/insectshader.frag", GL_FRAGMENT_SHADER, DEBUG_INFO);
   insectModules.push_back(insectFragModule);
   insectShader = makeShaderProgram(insectModules, DEBUG_INFO);

   std::vector<GLuint> soundWaveModules;
   soundWaveModules.push_back(loadShaderModule(getPath() + "data/shaders/soundwave.frag", GL_FRAGMENT_SHADER, DEBUG_INFO));
   soundWaveModules.push_back(loadShaderModule(getPath() + "data/shaders/soundwave.vert", GL_VERTEX_SHADER, DEBUG_INFO));
   soundWaveShader = makeShaderProgram(soundWaveModules, DEBUG_INFO);
}

double aspectRatio() { return static_cast<double>(Y_RESOLUTION)/static_cast<double>(X_RESOLUTION); }

void setupGraphics(bool doLoadShaders)
{
   // assumes SDL is already initialised
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, COLOUR_DEPTH );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
   screen = SDL_SetVideoMode(X_RESOLUTION, Y_RESOLUTION, COLOUR_DEPTH, SDL_OPENGL);
   if (!screen)
   {
      std::ostringstream error;
      error << "Failed on SDL_SetVideoMode: " << SDL_GetError() << "\n";
      throw std::runtime_error(error.str());
   }
   // no need for glViewPort (should be already set)
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
      
   glOrtho(-1.0/aspectRatio(),1.0/aspectRatio(), // left right
           -1.0,1.0, // top bottom
           1.0,-1.0); // clipping planes
   glMatrixMode(GL_MODELVIEW);

   glEnable( GL_TEXTURE_2D );
   glEnable(GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   getGLFunctions();
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   loadTextures();
   if (doLoadShaders)
     loadShaders(); // does nothing if shader's aren't available

   glClear(GL_COLOR_BUFFER_BIT);
   updateScreen();
}

void cleanupGraphics()
{
   // clear textures
   GLuint textures[] = { crowdTexture, playerTexture, valveTexture, numbersTexture, minusTexture, tickCrossTexture, beeTexture, butterflyTexture };
   //std::cout << sizeof(textures) << " " << sizeof(GLuint) << "\n";
   glDeleteTextures(sizeof(textures)/sizeof(GLuint), textures);

   const std::vector<GLuint>& smods = shaderModulesToBeDeleted();
   for (std::vector<GLuint>::const_iterator i = smods.begin(); i != smods.end(); ++i)
   {
      fpglDeleteShader(*i);
   }

   fpglDeleteProgram(insectShader);
   fpglDeleteProgram(soundWaveShader);
}

void draw(const double noteFraction)
{
   glLoadIdentity();
   // if it is the intro then zoom right in
   const Uint32 time = getTimeSinceStart(); // ms
   const double timeInSecs = time/1000.0;

   if (timeInSecs < ZOOM_OUT_LENGTH)
   {
      const double amountDone = (1.0-(timeInSecs/ZOOM_OUT_LENGTH));

      static const double MAX_ZOOM = 10.0;
      const double scaleFactor = 1.0*(1.0-amountDone) + MAX_ZOOM * amountDone;
      glScaled(scaleFactor, scaleFactor, 1.0);

      // centre on his face
      static const double Y_POS = 0.45;
      static const double X_POS = 0.05;
      glTranslated(X_POS*std::sqrt(amountDone), Y_POS*std::sqrt(amountDone), 0.0);
   }

   drawBackground();
   drawMan();
   drawInsects();
   drawTune(noteFraction);


   glLoadIdentity();
   writeNumber(score::getScore(), 0.98/aspectRatio(), -1.0, BOTTOM_RIGHT, 0.1);
}

void drawBackground()
{
   glDisable(GL_BLEND); // speed?

   static const double SCROLL_SPEED = 0.05; // screens per second

   Uint32 time = getTimeSinceStart(); // in millisecs
   const double timeInSecs = static_cast<double>(time)/1000;

   const double screensTraversed = SCROLL_SPEED * timeInSecs;

   const double texturePosition = std::fmod(screensTraversed,1.0)*4.0;

   const GLdouble textureCoords[] = { 4.0+texturePosition, 1.0,
                                    4.0+texturePosition, 0.0,
                                    texturePosition, 0.0,
                                    texturePosition, 1.0 };
   static const GLdouble HEIGHT = 1.1;
   static const GLdouble vertexCoords[] =  { 2.0, -HEIGHT, DRAW_DEPTH,
                                    2.0, HEIGHT, DRAW_DEPTH,
                                    -2.0, HEIGHT, DRAW_DEPTH,
                                    -2.0, -HEIGHT, DRAW_DEPTH };

   if (shadersViable && shadersActive)
   {
      static const GLint timeUni = fpglGetUniformLocation(soundWaveShader, "time");
      static const GLint textureSampler = fpglGetUniformLocation(soundWaveShader, "texture");
      static const GLint centreUni = fpglGetUniformLocation(soundWaveShader, "centre");
      static const GLint timeOfWavesUni = fpglGetUniformLocation(soundWaveShader, "timeOfWaves");

      static const GLfloat centrePos[] = { 0.15, -0.25, DRAW_DEPTH, 1.0 };

      const float* noteTimes = getNotePlayedTimes();

      fpglUseProgram(soundWaveShader);

      fpglUniform1f(timeUni, timeInSecs);
      fpglUniform4fv(centreUni, 1, centrePos);
      fpglUniform1fv(timeOfWavesUni, 16, noteTimes);
      fpglUniform1i(textureSampler, 0); // use default texture unit
   }


   glVertexPointer(3, GL_DOUBLE, 0, vertexCoords);
   glTexCoordPointer(2, GL_DOUBLE, 0, textureCoords);

   glBindTexture(GL_TEXTURE_2D, crowdTexture);

   glDrawArrays(GL_QUADS, 0, 4);

   glEnable(GL_BLEND);


   if (shadersViable) // this just turns them off, so doesn't matter if it's off
   {
      fpglUseProgram(0);
   }
}

void drawMan()
{
   static const double FRAMERATE = 5; // fps
   static const int NO_OF_FRAMES=4; // in animation sequence

   Uint32 time = getTimeSinceStart(); // in millisecs
   const double timeInSecs = static_cast<double>(time)/1000;

   const long framesTraversed = static_cast<long>(FRAMERATE * timeInSecs);

   const int activeFrame = framesTraversed % NO_OF_FRAMES;

   static const double TEXTURE_SIZE = 0.8; // same in all directions
   static const double TEXTURE_POS_X = 0.0;
   static const double TEXTURE_POS_Y = -0.5;

   const GLdouble textureCoords[] = { 0.25*static_cast<double>(activeFrame+1), 1.0,
                                      0.25*static_cast<double>(activeFrame+1), 0.0,
                                      0.25*static_cast<double>(activeFrame), 0.0,
                                      0.25*static_cast<double>(activeFrame), 1.0 };
   static const GLdouble vertexCoords[] = { TEXTURE_POS_X+(TEXTURE_SIZE/2.0),TEXTURE_POS_Y-(TEXTURE_SIZE/2.0),DRAW_DEPTH,
                                     TEXTURE_POS_X+(TEXTURE_SIZE/2.0),TEXTURE_POS_Y+(TEXTURE_SIZE/2.0),DRAW_DEPTH,
                                     TEXTURE_POS_X-(TEXTURE_SIZE/2.0),TEXTURE_POS_Y+(TEXTURE_SIZE/2.0),DRAW_DEPTH,
                                     TEXTURE_POS_X-(TEXTURE_SIZE/2.0),TEXTURE_POS_Y-(TEXTURE_SIZE/2.0),DRAW_DEPTH };

   glTexCoordPointer(2, GL_DOUBLE, 0, textureCoords);
   glVertexPointer(3, GL_DOUBLE, 0, vertexCoords);

   glBindTexture(GL_TEXTURE_2D, playerTexture);

   glDrawArrays(GL_QUADS, 0, 4);
}

void drawTune(const double noteFraction, const bool showGuide)
{
   static const double Y_POS = 0.75;
   static const double NOTE_SEPARATION = 0.8;

   static const double MAX_SIZE = 0.4;
   static const double SIZE_AT_EDGES = 0.2;

   // show an indication as to where we score if requested
   if ((showGuide) && (getTimeSinceStart() > ZOOM_OUT_LENGTH *1000.0 ))
   {
      glDisable(GL_TEXTURE_2D);

      static const double GUIDE_COLOUR[] = { 0.5, 0.5, 0.0 };
      static const double GUIDE_ALPHA_FAINTEST = 0.0;
      static const double GUIDE_ALPHA_DARKEST = 0.5;

      static const double HEIGHT = MAX_SIZE/2.0;

      static const GLdouble vertexCoords[] = { -NOTE_SEPARATION/2.0, Y_POS+HEIGHT, DRAW_DEPTH,
                                               -NOTE_SEPARATION/2.0, Y_POS-HEIGHT, DRAW_DEPTH,
                                               0.0, Y_POS+HEIGHT, DRAW_DEPTH,
                                               0.0, Y_POS-HEIGHT, DRAW_DEPTH,
                                               NOTE_SEPARATION/2.0, Y_POS+HEIGHT, DRAW_DEPTH,
                                               NOTE_SEPARATION/2.0, Y_POS-HEIGHT, DRAW_DEPTH };

      static const GLdouble colours [] = { GUIDE_COLOUR[0], GUIDE_COLOUR[1], GUIDE_COLOUR[2], GUIDE_ALPHA_FAINTEST,
                                           GUIDE_COLOUR[0], GUIDE_COLOUR[1], GUIDE_COLOUR[2], GUIDE_ALPHA_FAINTEST,
                                           GUIDE_COLOUR[0], GUIDE_COLOUR[1], GUIDE_COLOUR[2], GUIDE_ALPHA_DARKEST,
                                           GUIDE_COLOUR[0], GUIDE_COLOUR[1], GUIDE_COLOUR[2], GUIDE_ALPHA_DARKEST,
                                           GUIDE_COLOUR[0], GUIDE_COLOUR[1], GUIDE_COLOUR[2], GUIDE_ALPHA_FAINTEST,
                                           GUIDE_COLOUR[0], GUIDE_COLOUR[1], GUIDE_COLOUR[2], GUIDE_ALPHA_FAINTEST };


      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      glColorPointer(4, GL_DOUBLE, 0, colours);
      glVertexPointer(3, GL_DOUBLE, 0, vertexCoords);

      glDrawArrays(GL_QUAD_STRIP, 0, 6);

      glDisableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

      glEnable(GL_TEXTURE_2D);
      glColor4d(1.0, 1.0, 1.0 , 1.0);
   }

   const double currentNotePosition = (-noteFraction+0.5)*NOTE_SEPARATION;
   const double lastNotePosition = currentNotePosition - (NOTE_SEPARATION);
   const double last2ndNotePosition = currentNotePosition - (2.0*NOTE_SEPARATION);
   const double nextNotePosition = currentNotePosition + (NOTE_SEPARATION);
   const double next2ndNotePosition = currentNotePosition + (2.0*NOTE_SEPARATION);

   // sizeatedge = maxsize - decayfactor*sqrt(xatedge)
   // (-sizeatedge + maxsize) / sqrt(xatedge) = decayfactor
   const double DECAY_FACTOR = (MAX_SIZE - SIZE_AT_EDGES) / (1.0/aspectRatio());

   drawFingering(get2ndLastNote(), last2ndNotePosition, Y_POS, MAX_SIZE-DECAY_FACTOR*(std::sqrt(std::abs(last2ndNotePosition))),last2ndNoteHasBeenPlayed());
   drawFingering(getLastNote(), lastNotePosition, Y_POS, MAX_SIZE-DECAY_FACTOR*(std::sqrt(std::abs(lastNotePosition))),lastNoteHasBeenPlayed());
   drawFingering(getCurrentNote(), currentNotePosition, Y_POS, MAX_SIZE-DECAY_FACTOR*(std::sqrt(std::abs(currentNotePosition))), currentNoteHasBeenPlayed());
   drawFingering(getNextNote(), nextNotePosition, Y_POS, MAX_SIZE-DECAY_FACTOR*(std::sqrt(std::abs(nextNotePosition))), nextNoteHasBeenPlayed());
   drawFingering(get2ndNextNote(), next2ndNotePosition, Y_POS, MAX_SIZE-DECAY_FACTOR*(std::sqrt(std::abs(next2ndNotePosition))), next2ndNoteHasBeenPlayed());

}

void drawFingering(const char note, const double x, const double y, const double size, const NotePlayed played)
{
   const double top = y+(size/2.0);
   const double bottom = y-(size/2.0);

   if (note != NO_NOTE_CHARACTER) 
   {
      //Fingering f(true,false,true); // use this for now
      ReverseFingeringMap::const_iterator fIt = getReverseFingeringMap().find(note);
      if (fIt == getReverseFingeringMap().end())
      {  // I don't think this is exception worthy!
         std::cout << "Fingering for note " << note << " not found for drawing function.\n";
         return;
      }
      
      Fingering f(fIt->second);

      GLdouble vertexCoords[3*4*3]; // fill in as we go
      GLdouble textureCoords[2*4*3];
      
      // do each of the 3 keys
      for (int i=0; i<3; ++i)
      {
         // offsets for coordinates writing into the array
         const unsigned int vertOffset = 3*4*i;
         const unsigned int texOffset = 2*4*i;

         const bool keyiDown = (i == 0) ? f.key1() : ((i==1) ? f.key2() : f.key3());
   
         const double offset = keyiDown ? 0.5 : 0.0; // which half of texture to use
   
         // texture is twice as high as it is wide
         const double centreX = x+((i-1)*(size/2.0));
         const double left = centreX-(size/4.0);
         const double right = centreX+(size/4.0);

         // I think this can be made better by doing all the drawing in one go later!
         vertexCoords[vertOffset] = right; vertexCoords[vertOffset+1] = bottom; vertexCoords[vertOffset+2] = DRAW_DEPTH;
         vertexCoords[vertOffset+3] = right; vertexCoords[vertOffset+4] = top; vertexCoords[vertOffset+5] = DRAW_DEPTH;
         vertexCoords[vertOffset+6] = left; vertexCoords[vertOffset+7] = top; vertexCoords[vertOffset+8] = DRAW_DEPTH;
         vertexCoords[vertOffset+9] = left; vertexCoords[vertOffset+10] = bottom; vertexCoords[vertOffset+11] = DRAW_DEPTH;
         textureCoords[texOffset] = offset+0.5; textureCoords[texOffset+1] =  1.0;
         textureCoords[texOffset+2] = offset+0.5; textureCoords[texOffset+3] = 0.0;
         textureCoords[texOffset+4] = offset; textureCoords[texOffset+5] = 0.0;
         textureCoords[texOffset+6] = offset; textureCoords[texOffset+7]= 1.0;
      }

      glBindTexture(GL_TEXTURE_2D, valveTexture);

      glTexCoordPointer(2, GL_DOUBLE, 0, textureCoords);
      glVertexPointer(3, GL_DOUBLE, 0, vertexCoords);

      glDrawArrays(GL_QUADS, 0, 3*4);

   }

   if (played != NOT_YET_PLAYED)
   {
      // select offset for texture
      const double offset = (played == PLAYED_CORRECTLY) ? 0.0 : 0.5;
      glBindTexture(GL_TEXTURE_2D, tickCrossTexture);

      const GLdouble vertexCoords[] = { x+size, bottom, DRAW_DEPTH,
                                        x+size, top, DRAW_DEPTH,
                                        x-size, top, DRAW_DEPTH,
                                        x-size, bottom, DRAW_DEPTH };
      const GLdouble textureCoords[] = { offset+0.5, 1.0,
                                     offset+0.5, 0.0,
                                     offset, 0.0,
                                     offset, 1.0 };

      glTexCoordPointer(2, GL_DOUBLE, 0, textureCoords);
      glVertexPointer(3, GL_DOUBLE, 0, vertexCoords);

      glDrawArrays(GL_QUADS, 0, 4);
   }
}

void writeNumber(const signed int number, const double x, const double y, 
   const AnchorPosition pos, const double size)
{
   const bool negative = (number < 0) ? true : false; // implement later!

   const unsigned int absNumber = abs(number);

   std::vector<unsigned short> digits; // should only contain numbers between 0 and 9

   unsigned int powerOf10 = 1;
   unsigned int sumOfPreviousDigits = 0;

   while(absNumber >= (powerOf10))
   {
      unsigned short digit = ((absNumber % (powerOf10*10)) - sumOfPreviousDigits) / powerOf10;
      digits.push_back(digit);
      sumOfPreviousDigits += digit*powerOf10;
      powerOf10 *= 10;
   }

   if (digits.empty()) // score is zero
      digits.push_back(0);

   double rightX = x; // x on the right hand side
   if ((pos == TOP_LEFT) || (pos == BOTTOM_LEFT))
   {
      rightX += (size*digits.size()+negative)/2.0;
   }
   double centreY = y;
   if ((pos == TOP_LEFT) || (pos == TOP_RIGHT))
   {
      centreY -= size/2.0;
   }
   if ((pos == BOTTOM_RIGHT) || (pos == BOTTOM_LEFT))
   {
      centreY += size/2.0;
   }
 
   std::vector<GLdouble> texCoords; texCoords.reserve(digits.size()*4*2);
   std::vector<GLdouble> vertexCoords; vertexCoords.reserve(digits.size()*4*3);

   const std::vector<unsigned short>::const_iterator end = digits.end();
   for (std::vector<unsigned short>::const_iterator i = digits.begin(); i != end; ++i)
   {
      texCoords.push_back((static_cast<double>(*i)+1.0)/10.0); texCoords.push_back(1.0);
      vertexCoords.push_back(rightX); vertexCoords.push_back(centreY-(size/2.0)); vertexCoords.push_back(DRAW_DEPTH);
      texCoords.push_back((static_cast<double>(*i)+1.0)/10.0); texCoords.push_back(0.0);
      vertexCoords.push_back(rightX); vertexCoords.push_back(centreY+(size/2.0)); vertexCoords.push_back(DRAW_DEPTH);
      texCoords.push_back((static_cast<double>(*i))/10.0); texCoords.push_back(0.0);
      vertexCoords.push_back(rightX-size); vertexCoords.push_back(centreY+(size/2.0)); vertexCoords.push_back(DRAW_DEPTH);
      texCoords.push_back((static_cast<double>(*i))/10.0); texCoords.push_back(1.0);
      vertexCoords.push_back(rightX-size); vertexCoords.push_back(centreY-(size/2.0)); vertexCoords.push_back(DRAW_DEPTH);

      rightX -= size;
   }
   glBindTexture(GL_TEXTURE_2D, numbersTexture);
   glTexCoordPointer(2, GL_DOUBLE, 0, &texCoords[0]);
   glVertexPointer(3, GL_DOUBLE, 0, &vertexCoords[0]);

   glDrawArrays(GL_QUADS, 0, digits.size()*4);

   if (negative)
   {
      // to avoid rebinding I'm just going to overwrite the numbers
      // digits is guaranteed to be 1 so we have space
      texCoords[0] = 1.0; texCoords[1] = 1.0;
      texCoords[2] = 1.0; texCoords[3] = 0.0;
      texCoords[4] = 0.0; texCoords[5] = 0.0;
      texCoords[6] = 0.0; texCoords[7] = 1.0;
      vertexCoords[0] = rightX; vertexCoords[1] = centreY-(size/2.0); // vertexCoords 2,5 and 8 are already draw depth!
      vertexCoords[3] = rightX; vertexCoords[4] = centreY+(size/2.0);
      vertexCoords[6] = rightX-size; vertexCoords[7] = centreY+(size/2.0);
      vertexCoords[9] = rightX-size; vertexCoords[10] = centreY-(size/2.0);

      glBindTexture(GL_TEXTURE_2D, minusTexture);
      glDrawArrays(GL_QUADS, 0, 4);
   }

   //std::cout << " ";
}

void drawInsects()
{
   if (!(pointSpritesEnabled && shadersViable && shadersActive))
      return; // unsupported

   glEnable(GL_POINT_SPRITE);
   glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
   glPointSize(32.0);
   glBindTexture(GL_TEXTURE_2D, beeTexture); // bind the bee texture
   static class DoOnce
   {
      public:
         DoOnce()
         {
            glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
         }
   } doOnce;

   fpglUseProgram(insectShader);
   static const GLint rangesUni = fpglGetUniformLocation(insectShader, "ranges");
   //if (rangesUni == -1)
   //   std::cout << "Failed to find uniform \"ranges\".\n");
   static const GLint insectParamsUni = fpglGetUniformLocation(insectShader, "insectParams");
   //if (insectParamsUni == -1)
     // throw std::runtime_error("Failed to find uniform \"insectParams\".\n");
   static const GLint timeUni = fpglGetUniformLocation(insectShader, "time");
   static const GLint textureSampler = fpglGetUniformLocation(insectShader, "texture");
   static const GLint frameInfoUni = fpglGetUniformLocation(insectShader, "frameInfo");

   static const float speed = 1.0;
   static const float wobble = 0.05;
   static const float wobbleFreq = 8.0;
   static const float seed = static_cast<double>(std::rand())/static_cast<double>(RAND_MAX);
   const float time = static_cast<float>(getTimeSinceStart())/1000.0;
   static const GLfloat ranges[4] = { -1.0/aspectRatio(), 1.0/aspectRatio(),
                                   -1.0, 1.0 };

   //std::cout << time << "\n";

   static const GLfloat insectParams[] = { speed, wobbleFreq, wobble, seed,
                                     speed, wobbleFreq, wobble, seed+1.0,
                                     speed/1.5, wobbleFreq/2.0, wobble*4.0, seed+2.0 };

   //std::cout << seed << "\n";

   fpglUniform1i(textureSampler, 0); // use default texture unit
   fpglUniform1f(timeUni,time);
   static const float frameRate = 10.0;
   static const float noOfFrames = 2.0;
   fpglUniform2f(frameInfoUni,frameRate,noOfFrames);
   fpglUniform4fv(rangesUni, 1, ranges);
   fpglUniform4fv(insectParamsUni, 1, insectParams);
   glBegin(GL_POINTS);
   glVertex3d(0.0,0.0,DRAW_DEPTH);
   glEnd();

   fpglUniform4fv(insectParamsUni, 1, insectParams+4);
   glBegin(GL_POINTS);
   glVertex3d(0.0,0.0,DRAW_DEPTH);
   glEnd();

   glBindTexture(GL_TEXTURE_2D, butterflyTexture); // bind the bee texture
   fpglUniform4fv(insectParamsUni, 1, insectParams+8);
   glBegin(GL_POINTS);
   glVertex3d(0.0,0.0,DRAW_DEPTH);
   glEnd();

   // undo state changes
   fpglUseProgram(0);
   glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
   glDisable(GL_POINT_SPRITE);
}

void updateScreen()
{
   SDL_GL_SwapBuffers();
}

void toggleShaders()
{
   shadersActive = !shadersActive;
}

}
