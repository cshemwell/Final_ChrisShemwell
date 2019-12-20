For the design of this project, I went off the MonoSynth exmaple from class. I knew it had to be a playable patch, so the MonoSynth patch was the center of design.

To begin, I first needed to be able to access any Autopan components from any part of the code, so I made an Autopan struct (and then added that stuct to the monosynth struct).
To actually construct the autopan, I altered the process function in main.c. I first had to create a sine wave for the autopan itself to scroll through. I used the equation you helped write me in tutoring: 

                float theta = 0, sine = 0;
                    theta = synth->autoparams->phase * M_PI * 2.0f;
                    sine = sin(theta);

Then I could use: phase += rate to scroll through the sine function faster/slower.
From this point, you just multiple each sample by the current phase(interleaved of course) to get your autopan

                buffer[n] = sample * phase;
                buffer[n+1] = sample * phase;



Gain was as simple as:
                buffer[n] = sample * phase * Gain;
                buffer[n+1] = sample * phase * Gain;

Depth on the other hand, was more complex. I initially tried:
                                               sine = (kDepth *2) sin(theta)
                                    so that lower depth would = a lower amplitude sine wave.
                                    And to make up for the fact the ampltude would be 0 with 0 depth. I did:
                                                    buffer[n] = (sample * phase) + ((kDepth -1) * -1);
                                                    buffer[n+1] = (sample * phase) + ((kDepth -1) * -1);
                                    **This didn't work and both channels had equal phase (not autopan)
So the solution was:
                Amp = 1.0 + synth->autoparams->kDepth * sine;
                Amp2 = 1.0 - synth->autoparams->kDepth * sine;
                buffer[n] = Amp * sample * synth->autoparams->kGain;
                buffer[n+1] = Amp2 * sample * synth->autoparams->kGain;
I'm not sure I needed 2 Amps, but this was the only code I could get working, so I didn't alter it. 