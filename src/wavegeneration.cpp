#include "wavegeneration.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <iostream>

namespace wavegeneration {

Timbre::Timbre(std::istream& input) :
sineWaves(), squareWaves(), sawWaves(), triangleWaves()
{
   while (true)
   {
      std::string typeOfWave;
      double frequencyMult, volume;
      input >> typeOfWave >> frequencyMult >> volume;

      if(!input)
         break;

      if (typeOfWave == "sin" || typeOfWave == "sine")
         sineWaves.push_back(std::make_pair(frequencyMult, volume));
      else if (typeOfWave == "square" || typeOfWave == "sq")
         squareWaves.push_back(std::make_pair(frequencyMult, volume));
      else if (typeOfWave == "saw")
         sawWaves.push_back(std::make_pair(frequencyMult, volume));
      else if (typeOfWave == "triangle" || typeOfWave == "tr")
         triangleWaves.push_back(std::make_pair(frequencyMult, volume));
      else
         throw std::runtime_error(std::string("Invalid type of wave specified: ") + typeOfWave + std::string(" passed.\n"));
   }
   if (!input.eof()) // we've failed for someother reason
      throw std::runtime_error("Invalid input to timbre.\n");
}

std::vector<double> Timbre::getWave(const double frequency, const unsigned int reps) const
{
   // vector containing generated waves, and lengths
   std::vector<std::pair<std::vector<double>,double> > waves;

   typedef std::vector<FreqVol> VecFV;

   const VecFV::const_iterator sineEnd = sineWaves.end();
   for (VecFV::const_iterator i = sineWaves.begin(); i != sineEnd; ++i)
   {
      waves.push_back(std::make_pair(makeSineWave(frequency * i->first, reps),i->second));
   }

   const VecFV::const_iterator sqEnd = squareWaves.end();
   for (VecFV::const_iterator i = squareWaves.begin(); i != sqEnd; ++i)
   {
      waves.push_back(std::make_pair(makeSquareWave(frequency * i->first, reps),i->second));
   }

   const VecFV::const_iterator sawEnd = sawWaves.end();
   for (VecFV::const_iterator i = sawWaves.begin(); i != sawEnd; ++i)
   {
      waves.push_back(std::make_pair(makeSawWave(frequency * i->first, reps),i->second));
   }

   const VecFV::const_iterator triangleEnd = triangleWaves.end();
   for (VecFV::const_iterator i = triangleWaves.begin(); i != triangleEnd; ++i)
   {
      waves.push_back(std::make_pair(makeTriangleWave(frequency * i->first, reps),i->second));
   }

   return mixWaves(waves);
}

std::vector<double> makeWave(double (*funPtr)(const double, const double), const double frequency, const int reps)
{
   const double lengthInSecs = reps/frequency;
   const unsigned long lengthInSamples = 
      static_cast<unsigned long>(lengthInSecs * AUDIO_RATE);
   std::vector<double> data(lengthInSamples);

   const std::vector<double>::iterator begin = data.begin();
   const std::vector<double>::iterator end = data.end();
   for (std::vector<double>::iterator i = begin; i<end; ++i)
   {
      const double time = static_cast<double>(i - begin)/AUDIO_RATE;
      *i = funPtr(time, frequency);
   }

   return data;
}

double sineWaveFunc(const double time, const double freq)
{
   static const double PI = std::acos(-1.0);
   return std::sin(2.0*PI*time*freq);
}

double squareWaveFunc(const double time, const double freq)
{
   return (sineWaveFunc(time,freq) > 0) ? 1.0 : -1.0;
}

double sawWaveFunc(const double time, const double freq)
{
   const double timePeriod = 1.0 / freq;
   const double mod = std::fmod(time,timePeriod);
   const double frac = mod / timePeriod;

   return frac*2.0 - 1.0;
}

double triangleWaveFunc(const double time, const double freq)
{
   const double timePeriod = 1.0 / freq;
   const double mod = std::fmod(time,timePeriod);
   const double frac = mod / timePeriod;

   if (frac > 0.5)
      return -1.0 + 4.0*(1.0-frac);
   else
      return -1.0 + 4.0*frac;
}

std::vector<SoundDataType> convertToSoundFormat(const std::vector<double>& dataIn)
{
   std::vector<SoundDataType> dataOut(dataIn.size());

   for (unsigned long i = 0; i < dataOut.size(); ++i)
   {
      const double value = dataIn[i];
      if (std::abs(value) > 1.0)
      {
         std::ostringstream error;
         error << "Value of " << value << " is outside the required range (-1 to 1).\n"
               << "in index " << i << " of " << dataIn.size() << "\n";
         throw std::logic_error(error.str());
      }

      static const double SOUND_DATA_TYPE_MIN = 
         static_cast<double>(std::numeric_limits<SoundDataType>::min());
      static const double SOUND_DATA_TYPE_MAX = 
         static_cast<double>(std::numeric_limits<SoundDataType>::max());
      if (value < 0.0)
         dataOut[i] = static_cast<SoundDataType>(std::abs(value) *  SOUND_DATA_TYPE_MIN/2);
      else
         dataOut[i] = static_cast<SoundDataType>(value *  SOUND_DATA_TYPE_MAX/2);
      //std::cout << dataOut[i] << "\n";
   }

   return dataOut;
}

std::vector<double> mixWaves(const std::vector<std::pair<std::vector<double>,double> >& waves)
{
   typedef std::vector<std::pair<std::vector<double>,double> > VecType;
   double sumOfRatios = 0;
   unsigned long greatestSize = 0;
   const VecType::const_iterator end = waves.end();
   for (VecType::const_iterator i = waves.begin(); i!=end; ++i)
   {
      if (i->first.size() > greatestSize)
         greatestSize = i->first.size();

      sumOfRatios += i->second;
   }

   std::vector<double> result(greatestSize);
   for (unsigned long i = 0; i < greatestSize; ++i)
   {
      double value = 0;
      for (VecType::const_iterator j = waves.begin(); j!=end; ++j)
      {
         std::size_t index = i;
         const std::size_t size = j->first.size();
         index %= size; // make size an acual number in range

         value += j->first[index] * (j->second/sumOfRatios);
      }

      result[i] = value;
   }

   return result;
}

Mix_Chunk* createMixChunk(const std::vector<SoundDataType>& data)
{
   // use malloc here, because it will be freed by SDL with malloc
   SoundDataType* internalData = reinterpret_cast<SoundDataType*>(malloc(data.size() * sizeof(SoundDataType)));
   if (!internalData)
      throw std::runtime_error("Failed to allocated memory for Mix_Chunk\n");

   // copy the data across
   std::copy(data.begin(), data.end(), internalData);

   Mix_Chunk* blankMixChunk = reinterpret_cast<Mix_Chunk*>(malloc(sizeof(Mix_Chunk)));
   blankMixChunk->allocated = 0;
   blankMixChunk->abuf = reinterpret_cast<Uint8*>(internalData);
   blankMixChunk->alen = data.size() * sizeof(SoundDataType);
   blankMixChunk->volume = MIX_MAX_VOLUME; // loudest*/

   return blankMixChunk;
}

}
