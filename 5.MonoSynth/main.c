#include "MIDI.h"
#include "Audio.h"
#include "MonoSynth.h"

//Compile with:
//make
//Run with:
//make run

#define kMIDIAmpIncrement 0.007874016f

void getMIDIInput(Synth *synth);
void process(float *buffer, unsigned long *numFrames, Synth *synth);

int main(void){
  //Initialize the synthesizer
  Synth *synth = (Synth *) malloc(sizeof(Synth));
  if(initSynth(synth)) return 1;

  //Open audio stream
  AudioStream *audioStream;
  audioStream = (AudioStream *) malloc(sizeof(AudioStream));

  if(initPortAudio()) return 1;
  //printPaDevices();
  if(openAudioStream(audioStream, renderCallback, synth)) return 1;

  // Start audio processing
  if(startAudioStream(audioStream)) return 1;
  // End audio processing
  if(closePortAudio()) return 1;

  //Close MIDI stream
  if(closePortMIDI(synth->midiStream)) return 1;

  //Clean up
  closeSynth(synth);

  if(audioStream){
    free(audioStream);
  }
}

int renderCallback(
  const void *input,
  void *output,
  unsigned long frameCount,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData 
){
  float *inBuffer = (float *) input;
  float *outBuffer = (float *) output;
  unsigned long numFrames = frameCount * kNumChannels;
  Synth *synth = (Synth *) userData;

  getMIDIInput(synth);

  if(synth->midiEvent.isOn && !synth->envelope->gate){
    //Turn on envelope
    synth->envelope->gate = true;
    //Turn midi note off
    synth->midiEvent.isOn = false;
    setAmplitude(synth->wavetable,synth->midiEvent.velocity*kMIDIAmpIncrement);
  }
  else if(synth->midiEvent.isOn && synth->envelope->gate){ //retrigger the envelope
    reset(synth->envelope);
  }

  //Set frequency regardless of envelope state
  setFrequency(synth->wavetable, &synth->midiEvent.note);

  if(synth->envelope->gate){
    process(outBuffer, &numFrames, synth);    
  }
  else{ //Silence
    for(unsigned long n=0; n < numFrames; n+=kNumChannels){
      for(int c=0;c<kNumChannels;c++){
        outBuffer[n+c] = 0;
      }
    }
  }

  return 0;
}

void process(float *buffer, unsigned long *numFrames, Synth *synth){
  static float sample = 0;
  if(synth->wavetable->isPlaying){
    for(unsigned long n=0; n < *numFrames; n+=kNumChannels){
      sample = synth->wavetable->amplitude * next(synth->wavetable) * getCurrentAmp(synth->envelope);
      for(int c=0;c<kNumChannels;c++){
        buffer[n+c] = sample;
      }
    }
  }
}

void getMIDIInput(Synth *synth){
  static int numMIDIEvents;
  static char velocity;
  static char note;
  MIDIStream *midiStream = synth->midiStream;
  
  if(Pm_Poll(midiStream->input)){
    numMIDIEvents = Pm_Read(midiStream->input, midiStream->events, kMaxMIDIEvents);
    for(int i=0; i < numMIDIEvents; i++){
      note = Pm_MessageData1(midiStream->events[i].message);
      velocity = Pm_MessageData2(midiStream->events[i].message);
      if(velocity != 0){
        synth->midiEvent.note = note;
        synth->midiEvent.velocity = velocity;
        synth->midiEvent.isOn = true;
      }
    }
  }
}
