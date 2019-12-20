#include <stdio.h>
#include <portmidi.h>
#include <porttime.h>

//Compile/Run with:
//gcc MIDIThrough.c -o MIDI -lportmidi && ./MIDI

#define kMIDIInputDeviceID 0
#define kMIDIOutputDeviceID 2
#define kMaxMIDIEvents 32

int main(void){
  PmError error; //Store error codes returned by PortMIDI functions
  int numDevices; //Store the number of MIDI device available
  PmDeviceID curDeviceID; //current device ID

  const PmDeviceInfo *pDeviceInfo; //Provides information and capabilities of PortMIDI devices
  int numMIDIEvents = 0; //Store a count for all incoming MIDI events
  PmEvent midiEvents[kMaxMIDIEvents]; //Hold individual MIDI events
  PortMidiStream *inputStream; //a descriptor for an open MIDI input device
  PortMidiStream *outputStream; //a descriptor for an open MIDI output device
  
  PtError ptError; //Store error codes returned by PortTime functions

  //Initialize Port MIDI
  error = Pm_Initialize();
  if(error != pmNoError){
    printf("Error: Pm_Initialize() failed. %s\n", Pm_GetErrorText(error));
    return 1;
  }

  //Get the number of port MIDI devices available
  numDevices = Pm_CountDevices();

  //Iterate over each device and print out information about them
  for(curDeviceID = 0; curDeviceID < numDevices; curDeviceID++){
    pDeviceInfo = Pm_GetDeviceInfo(curDeviceID);
    printf("--------------------------------------------\n");
    printf("ID: %d, Name: %s, ",curDeviceID,pDeviceInfo->name);
    printf("MIDI API: %s\n",pDeviceInfo->interf);
    printf("Max MIDI outputs: %d\t",pDeviceInfo->output);
    printf("Max MIDI inputs: %d\n\n",pDeviceInfo->input);
  }

  //Start a timer
  ptError = Pt_Start(1, NULL, NULL);//arguments are Resolution, callback function, and user data
  if(ptError != ptNoError){
    printf("Error: Pt_Start() failed with error code %d\n",ptError);
    return 1;
  }

  //Open MIDI input
  error = Pm_OpenInput(&inputStream, kMIDIInputDeviceID, NULL, 512L, NULL, NULL);
  if(error != pmNoError){
    printf("Error: Pm_OpenInput() failed. %s\n", Pm_GetErrorText(error));
    return 1;
  }

  //Open MIDI output
  error = Pm_OpenOutput(&outputStream, kMIDIOutputDeviceID, NULL, 512L, NULL, NULL,0);
  if(error != pmNoError){
    printf("Error: Pm_OpenOutput() failed. %s\n", Pm_GetErrorText(error));
    return 1;
  }

  //Run MIDI stream for 10 second
  while(Pt_Time(NULL) < 10000){
    if(Pm_Poll(inputStream)){
      numMIDIEvents = Pm_Read(inputStream, midiEvents, kMaxMIDIEvents);
      for(int i=0; i < numMIDIEvents;i++){
        // Comment printf() out for a faster response
        printf("status: 0x%d, byte1: %d, byte2: %d, time: %.3f\n",
          Pm_MessageStatus(midiEvents[i].message),//MIDI status
          Pm_MessageData1(midiEvents[i].message),//Note Number
          Pm_MessageData2(midiEvents[i].message),//Velocity
          midiEvents[i].timestamp/1000.);

          Pm_WriteShort(outputStream, 0, Pm_Message(
            Pm_MessageStatus(midiEvents[i].message),
            Pm_MessageData1(midiEvents[i].message),
            Pm_MessageData2(midiEvents[i].message)
          ));
      }
    }
  }

  //Close input MIDI stream
  error = Pm_Close(inputStream);
  if(error != pmNoError){
    printf("Error: Pm_Close() failed. %s\n",Pm_GetErrorText(error));
    return 1;
  }

  //Close output MIDI stream
  error = Pm_Close(outputStream);
  if(error != pmNoError){
    printf("Error: Pm_Close() failed. %s\n",Pm_GetErrorText(error));
    return 1;
  }

  //Terminate Port MIDI
  error = Pm_Terminate();
  if(error != pmNoError){
    printf("Error: Pm_Terminate() failed. %s\n",Pm_GetErrorText(error));
    return 1;
  }

  //Stop the timer
  ptError = Pt_Stop();//arguments are Resolution, callback function, and user data
  if(ptError != ptNoError){
    printf("Error: Pt_Stop() failed with error code %d\n",ptError);
    return 1;
  }
}