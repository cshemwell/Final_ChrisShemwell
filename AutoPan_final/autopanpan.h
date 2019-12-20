#pragma once
#include <stdbool.h>
#include "Audio.h"

// #define kDepth 1.0
// #define kRate 0.2 //Hz
// #define kGain 0.5
// #define kNumChannels 2

typedef struct AutoParams{
    float kDepth;
    float kRate; 
    float kGain; 
    float phase;
    //float kNumChannels;
} AutoParams;

bool initParams(AutoParams *autoparams);




