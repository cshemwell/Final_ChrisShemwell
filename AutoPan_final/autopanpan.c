#include "autopanpan.h"

// initialize parameters so main knows they exist
//where should I put this in main?
bool initParams(AutoParams *autoparams){
    //AutoParams *autoparams;
    autoparams->kDepth = 1.0;
    autoparams->kRate = 0.2 / kSamplingRate;
    autoparams->kGain = 0.5;
    autoparams->phase = 0;
    return false;
}