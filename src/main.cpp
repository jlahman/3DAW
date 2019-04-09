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
        if (ap->start())
        {
            printf("Play for %d seconds.\n", NUM_SECONDS );

            Pa_Sleep( NUM_SECONDS * 1000 );

            ap->stop();
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
