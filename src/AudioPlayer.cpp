/*
* Skeleton Example code was taken from paex_sine_c++.cpp
  TODO: should this be purely an audio player? Take out all data processing and put that in a seperate class?
        How to get the final buffer data to the audio player?
        If not realtime could have a function to change pointer to data? would realtime make be any different?
 */
#include<iostream>
#include"AudioPlayer.h"

AudioPlayer::AudioPlayer()
{
    /* initialise variables */
    timingCounter = 0;
    trackList.push_back(new Track("../data/narrator2.ogg"));
    trackList.push_back(new Track("../data/test.wav"));
    trackList.push_back(new Track("../data/narrator.ogg"));

    anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_011/hrir_final.mat");
    anime->addSource("Narrator2", trackList[0]);
    anime->addKeyFrame("Narrator2",0.0,  new SoundSourceProperties(new Polar3D(1.0, -100, 0.0), false, true));
    anime->addSource("Waterfall", trackList[1]);
    anime->addSource("Narrator", trackList[2]);

    anime->setStartTime("Narrator", 5.0);
    anime->setStartTime("Narrator2", 0.0);

    anime->test_KeyFrames("Narrator");
}

bool AudioPlayer::open(PaDeviceIndex indexx)
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

bool AudioPlayer::close()
{
    if (stream == 0)
        return false;

    PaError err = Pa_CloseStream( stream );
    stream = 0;

    return (err == paNoError);
}


bool AudioPlayer::start()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StartStream( stream );

    return (err == paNoError);
}

bool AudioPlayer::restart()
{
    if (stream == 0)
        return false;

    timingCounter = 0;
    return true;
}

bool AudioPlayer::stop()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StopStream( stream );

    return (err == paNoError);
}
/* The instance callback, where we have access to every method/variable in object of class Sine */
int AudioPlayer::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
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
    //printf("%E\n", timingCounter/44100.0);
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
//was static, check pa docs
int AudioPlayer::paCallback( const void *inputBuffer, void *outputBuffer,
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


void AudioPlayer::paStreamFinishedMethod()
{
  //  printf( "Stream Completed\n" );
  return;
}

/*
 * This routine is called by portaudio when playback is done.
 */
 //was static, check pa docs
 void AudioPlayer::paStreamFinished(void* userData)
{
    return ((AudioPlayer*)userData)->paStreamFinishedMethod();
}
