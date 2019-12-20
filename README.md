-----Real Time SquareWave Synth with Autopan-----
             ~Chris Shemwell~
    
    //This code will generate a square wave synth patch you can play with your MIDI keyboard. The square wave output is run through an autopan proccessor, which has 3 real time controls of Depth, Rate, and Gain//

            By default, this code will be using your 
                -built in speakers for output
                -first MIDI device it recieves for input
                -CC 20 will control autopan depth
                -CC 21 will control autopan rate
                -CC 22 will control gain


             (If you'd like to change these settings, please compile/run the MIDIthrough.c and/or AudioDeviceList.c to first see which input/outputs you'd like to be using, then change those corresponding numbers in the Audio.h and MIDI.h #define section. To use your own MIDI device's knobs, again run the MIDIthrough.c and direct your eyes towards the "byte1: __" messages, as this will tell you the correct CC knob # you are currently twisting. From this point, you can go to lines 14-16 of main.c and change Knobs 1-3 accordingly.)

To compile type: make
To run the patch type: ./main

NOW THAT CODE IS RUNNING..
Play any MIDI note to activate a sustained note. This is only a monophonic sustaining instrument.
Twist Knobs 1-3 to change the Autopan parameters:
Knob1 = Depth
Knob2 = Rate
Knob3 = Gain

to exit type: (Control + C)



--------------------------------------------------------------------------------------------------------