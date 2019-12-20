#include <stdbool.h>

#define kNumPoints 5
#define kMaxAmp 1.0f

typedef struct Point {
  float x;
  float y;
} Point;

typedef struct Envelope {
  bool gate;
  bool sustain;
  Point points[kNumPoints];
  int curIndex;
  int curFrame;
  int totalFrames; //sampleRate * duration
  float sampleRate;
  float duration; //Duration of envelope in second
  float max; // Max y value
} Envelope;

// Function Prototypes
void initEnvelope(Envelope *envelope, float duration, float sampleRate);
void reset(Envelope *envelope);
float getCurrentAmp(Envelope *envelope);
void setDuration(Envelope *envelope, float duration);