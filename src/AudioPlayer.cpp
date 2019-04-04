/*
* Skeleton Example code was taken from paex_sine_c++.cpp
  TODO: should this be purely an audio player? Take out all data processing and put that in a seperate class?
        How to get the final buffer data to the audio player?
        If not realtime could have a function to change pointer to data? would realtime make be any different?
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cmath>
#include<matio.h>
#include<portaudio.h>


#include"HRIR_Data.h"
#include"Track.h"
#include"util.h"
#include"AnimationPlayer.h"

#define NUM_SECONDS   (10)


#define FRAMES_PER_BUFFER  (2048) //needs to be at least 201 for no errors: 2048 seems to be as low as we can go rn

class AudioPlayer
{
public:
      AudioPlayer()
      {
          /* initialise variables */
          timingCounter = 0;
          trackList.push_back(new Track("../data/Bee.mp3"));
          trackList.push_back(new Track("../data/test.wav"));
          anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_033/hrir_final.mat");
          //anime->addSource("Bee", trackList[0]);
          anime->addSource("Waterfall", trackList[1]);
          //hrir = new HRIR_Data("../data/CIPIC_hrtf_database/standard_hrir_database/subject_162/hrir_final.mat");

    }

    bool open(PaDeviceIndex indexx)
    {
        PaStreamParameters outputParameters;

        outputParameters.device = indexx;
        if (outputParameters.device == paNoDevice) {
            return false;
        }

        outputParameters.channelCount = 2;       /* stereo output */
        outputParameters.sampleFormat = paFloat32;  //paInt16; /* 32 bit float output */
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        PaError err = Pa_OpenStream(
            &stream,
            NULL, /* no input */
            &outputParameters,
            44100,
            FRAMES_PER_BUFFER,//22050/8,
            paNoFlag,
            &AudioPlayer::paCallback,
            this            /* Using 'this' for userData so we can cast to AudioProcessor* in paCallback method */
            );

        if (err != paNoError)
        {
            /* Failed to open stream to device !!! */
            return false;
        }

        err = Pa_SetStreamFinishedCallback( stream, &AudioPlayer::paStreamFinished );

        if (err != paNoError)
        {
            Pa_CloseStream( stream );
            stream = 0;

            return false;
        }

        return true;
    }

    bool close()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_CloseStream( stream );
        stream = 0;

        return (err == paNoError);
    }


    bool start()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StartStream( stream );

        return (err == paNoError);
    }

    bool stop()
    {
        if (stream == 0)
            return false;

        PaError err = Pa_StopStream( stream );

        return (err == paNoError);
    }

private:
    /* The instance callback, where we have access to every method/variable in object of class Sine */
    int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags)
    {
        unsigned long i;
        float* out = (float *) outputBuffer;

        //double bufferLeft[framesPerBuffer];
        //double bufferRight[framesPerBuffer];
        
        double **buffer = new double*[2];
        buffer[0] = new double[FRAMES_PER_BUFFER];
        buffer[1] = new double[FRAMES_PER_BUFFER];

        anime->getBuffer(buffer, timingCounter, framesPerBuffer);

        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = buffer[0][i];
            *out++ = buffer[1][i];
        }

        //Just to keep circling while animation player is WIP
        /*double newAzi = trackList[0]->getAzimuth() + 180 + 5;
        newAzi = fmod(newAzi, 360);
        newAzi -= 180;
        trackList[0]->setAzimuth(newAzi);*/

        timingCounter = timingCounter + framesPerBuffer -199;
       // index = index + framesPerBuffer;
        (void) timeInfo; /* Prevent unused variable warnings. */
        (void) statusFlags;
        (void) inputBuffer;

        return paContinue;

    }

    /* This routine will be called by the PortAudio engine when audio is needed.
    ** It may called at interrupt level on some machines so don't do anything
    ** that could mess up the system like calling malloc() or free().
    */
    static int paCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData )
    {
        /* Here we cast userData to AudioProcessor* type so we can call the instance method paCallbackMethod, we can do that since
           we called Pa_OpenStream with 'this' for userData */


        return ((AudioPlayer*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
            framesPerBuffer,
            timeInfo,
            statusFlags);

    }


    void paStreamFinishedMethod()
    {
        printf( "Stream Completed\n" );
    }

    /*
     * This routine is called by portaudio when playback is done.
     */
    static void paStreamFinished(void* userData)
    {
        return ((AudioPlayer*)userData)->paStreamFinishedMethod();
    }


    PaStream *stream;
    AnimationPlayer * anime;
    //HRIR_Data *hrir;
    int timingCounter;
    char message[20];
    std::vector<Track*> trackList;

};


/*******************************************************************/
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
