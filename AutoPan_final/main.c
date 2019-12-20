#include "MIDI.h"
#include "Audio.h"
#include "MonoSynth.h"
#include "autopanpan.h"
#include <sndfile.h>
#include <portmidi.h>


//Compile with:
//make
//Run with:
//make run
#define kMIDIAmpIncrement 0.007874016f
#define Knob1 20
#define Knob2 21
#define Knob3 22

void getMIDIInput(Synth *synth);
void process(float *buffer, unsigned long *numFrames, Synth *synth);

int main(void){
//Initialize the synthesizer
  Synth *synth = (Synth *) malloc(sizeof(Synth));
  if(initSynth(synth)) return 1;


  synth->autoparams = (AutoParams *) malloc(sizeof(AutoParams));
    
  //Open audio stream
  AudioStream *audioStream;
  audioStream = (AudioStream *) malloc(sizeof(AudioStream));

  if(initPortAudio()) return 1;
  //printPaDevices();
  if(openAudioStream(audioStream, renderCallback, synth)) return 1;

  //-----------------adding initParams here?
  if(initParams(synth->autoparams)) return 1;

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
  double Amp, Amp2;
  if(synth->wavetable->isPlaying){
    for(unsigned long n=0; n < *numFrames; n+=kNumChannels){
      sample = synth->wavetable->amplitude * next(synth->wavetable) * getCurrentAmp(synth->envelope);
      
      float theta = 0, sine = 0;
      theta = synth->autoparams->phase * M_PI * 2.0f;
      sine = sin(theta);
      //printf("this%f\n", sine);

  
      
      
      Amp = 1.0 + synth->autoparams->kDepth * sine;
      Amp2 = 1.0 - synth->autoparams->kDepth * sine;
      buffer[n] = Amp * sample * synth->autoparams->kGain;
      buffer[n+1] = Amp2 * sample * synth->autoparams->kGain;

      
      synth->autoparams->phase += (synth->autoparams->kRate * 1.0);
      if(synth->autoparams->phase >= 1.0f){
        synth->autoparams->phase = -1.0f;
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
      if(Pm_MessageStatus(midiStream->events[i].message) >= 0x90 && Pm_MessageStatus(midiStream->events[i].message) <= 0x9f){
        note = Pm_MessageData1(midiStream->events[i].message);
        velocity = Pm_MessageData2(midiStream->events[i].message);

    //printf("%d\n", Pm_MessageData1(midiStream->events[i].message));
        if(velocity != 0){
          synth->midiEvent.note = note;
          synth->midiEvent.velocity = velocity;
          synth->midiEvent.isOn = true;
        }
         else{
          synth->envelope->sustain = false;
        }
      }
      else {
        //------------------------------------------------------------------------------------------------------------
        //aassign knobs here        //my MIDI keyboard knobs CC are 20-23
        if(Pm_MessageData1(midiStream->events[i].message) == Knob1){
          
          synth->autoparams->kDepth = Pm_MessageData2(midiStream->events[i].message) / 127.;
        }
        else if(Pm_MessageData1(midiStream->events[i].message) == Knob2){
        synth->autoparams->kRate = (440.0f * pow(2, (Pm_MessageData2(midiStream->events[i].message) - 69.0f)/12.0f))/(kSamplingRate * 50.);
        }
        else if(Pm_MessageData1(midiStream->events[i].message) == Knob3){
        synth->autoparams->kGain = Pm_MessageData2(midiStream->events[i].message) / 127.;
        }
        
        
      }
    }
  }
}
