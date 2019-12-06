#pragma once

#include "MIDI.h"
#include "Wavetable.h"
#include "Envelope.h"

#define kNoteDuration 1

typedef struct Synth {
  Wavetable *wavetable;
  Envelope *envelope;
  MIDIStream *midiStream;
  MIDIEvent midiEvent;
} Synth;

// Function Prototypes
int initSynth(Synth *synth);
void closeSynth(Synth *synth);