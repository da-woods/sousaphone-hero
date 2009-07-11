#include "wavegeneration_fwd.hpp"

#ifndef WAVEGENERATION_HPP
#define WAVEGENERATION_HPP

#include "SDL_mixer.h"

#include "general.hpp"

// std includes
#include <vector>
#include <istream>

namespace wavegeneration {

// defines the mix of different waves making an instrument
class Timbre
{
   public:
      // construct from a file
      explicit Timbre(std::istream& input);

      // gets a wave, using the various properties given
      std::vector<double> getWave(const double frequency, const unsigned int reps = DEFAULT_REPS) const;
   private:
      // a pair which stores the frequency multiplication of the base note,
      // and the volume that frequency multiplication should be played at
      typedef std::pair<double,double> FreqVol;

      std::vector<FreqVol> sineWaves;
      std::vector<FreqVol> squareWaves;
      std::vector<FreqVol> sawWaves;
      std::vector<FreqVol> triangleWaves;

      static const unsigned int DEFAULT_REPS = 50;
};

// makes a wave (vector of doubles from -1 to 1 stored at correct frequency)
// based on input to function pointer (takes doubles time and frequency)
std::vector<double> makeWave(double (*funPtr)(const double, const double), const double frequency, const int reps = 1);

double sineWaveFunc(const double time, const double freq);

double squareWaveFunc(const double time, const double freq);

double sawWaveFunc(const double time, const double freq);

double triangleWaveFunc(const double time, const double freq);

// makes a sine wave of reps repetition
inline std::vector<double> makeSineWave(const double frequency, const int reps = 1)
{
   return makeWave(sineWaveFunc, frequency, reps);
}

inline std::vector<double> makeSquareWave(const double frequency, const int reps = 1)
{
   return makeWave(squareWaveFunc, frequency, reps);
}

inline std::vector<double> makeSawWave(const double frequency, const int reps = 1)
{
   return makeWave(sawWaveFunc, frequency, reps);
}

inline std::vector<double> makeTriangleWave(const double frequency, const int reps = 1)
{
   return makeWave(triangleWaveFunc, frequency, reps);
}

// mixes waves in prescribed ratio (ratios can be any number)
// pair of wave and ratio
std::vector<double> mixWaves(const std::vector<std::pair<std::vector<double>,double> >& waves);

/// converts an array of double from -1 to 1 to the sound data format
std::vector<SoundDataType> convertToSoundFormat(const std::vector<double>& dataIn);

Mix_Chunk* createMixChunk(const std::vector<SoundDataType>& data);

inline Mix_Chunk* createMixChunk(const std::vector<double>& data)
{
   return createMixChunk(convertToSoundFormat(data));
}


}

#endif
