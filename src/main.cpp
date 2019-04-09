#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include<portaudio.h>


#include"Track.h"
#include"util.h"
#include"AnimationPlayer.h"
#include"AudioPlayer.h"

#define NUM_SECONDS   (16)


int main(int argc,  char * argv[])
{
    PaError err;
    AudioPlayer* ap;

   ap = new AudioPlayer();

    printf("PortAudio Test\n");

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    if (ap->open(Pa_GetDefaultOutputDevice()))
    {
        char input;
        bool done = false;
        bool pause = false;
        while(!done){
          std::cin >> input;
          if(input == 'p'){
            pause = !pause;
          } else if(input == 'r') {
            ap->restart();
          } else if(input == 'q') {
            done = true;
          }
          if (pause)
          {
             ap->start();

              printf("Play\n" );
          } else {
            printf("Pause\n" );
            ap->stop();
          }
        }
        ap->close();
    }

    Pa_Terminate();
    printf("Test finished.\n");

    return err;

error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
