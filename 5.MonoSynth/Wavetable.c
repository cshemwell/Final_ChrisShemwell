#include "Wavetable.h"

void createWavetable(Wavetable *wavetable){
  //Square wave approximation
  for(int i = 1; i <= 15; i+=2){ //Up to 15th harmonics, but only odd harmonics
    double delta = (2.0 * M_PI) / (wavetable->size - 1) * i;
    double angle = 0.0;
    for (int n = 0; n < wavetable->size; n++){
      wavetable->table[n] += (1.0f / i) * sin(angle);
      angle += delta;
    }
  }
}

void initWavetable(Wavetable *wavetable){
  wavetable->table = (float *) calloc(kTableSize, sizeof(float));
  wavetable->size = kTableSize;
  wavetable->delta = kDefaultFrequency * (kTableSize / kSamplingRate);//set frequecy
  wavetable->curIndex = 0.0f;
  wavetable->isPlaying = true;
  wavetable->amplitude = 1.0f;
  createWavetable(wavetable);
}

float next(Wavetable *wavetable){
  unsigned long index0 = (unsigned long) wavetable->curIndex;
  unsigned long index1 = index0 + 1;
  
  //Calculate the interpolation value
  float fraction = wavetable->curIndex - (float)index0;
  
  //get values from the wavetable
  float value0 = wavetable->table[index0];
  float value1 = wavetable->table[index1];
  
  //Calculate the interpolated value
  float sample = value0 + fraction * (value1 - value0);
  
  //increment the angle delta of the table and wrap
  wavetable->curIndex += wavetable->delta;
  if(wavetable->curIndex >= (float) wavetable->size){
      wavetable->curIndex -= (float) wavetable->size;
  }
  
  return sample;
}

void setFrequency(Wavetable *wavetable, char *midi){
  static char m = 0;
  if(m != *midi){
    float frequency = (440.0f * pow(2, (*midi - 69.0f)/12.0f));
    wavetable->delta = frequency * (kTableSize / kSamplingRate);
    m = *midi;
  }
}

void setAmplitude(Wavetable *wavetable, float amplitude){
  wavetable->amplitude = amplitude;
}