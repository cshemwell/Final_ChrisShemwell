#include "MonoSynth.h"

int initSynth(Synth *synth){  
  synth->wavetable = (Wavetable *) malloc(sizeof(Wavetable));
  initWavetable(synth->wavetable);

  synth->envelope = (Envelope *) malloc(sizeof(Envelope));
  initEnvelope(synth->envelope, kNoteDuration, kSamplingRate);

  //Initialize MIDI
  synth->midiStream = (MIDIStream *) malloc(sizeof(MIDIStream));
  if(initPortMIDI(synth->midiStream, true, false)) return 1;

  return 0;
}

void closeSynth(Synth *synth){
  if(synth){
    free(synth->wavetable);
    free(synth->envelope);
    free(synth->midiStream);
    free(synth);
  }
}
