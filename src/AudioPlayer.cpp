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
    trackList.push_back(new Track("../data/souldfire.wav"));


    anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_058/hrir_final.mat");
    //anime->addSource("Narrator2", trackList[0]);
    anime->addKeyFrame("Narrator2",0.0,  new SoundSourceProperties(new Polar3D(1.0, -100, 0.0), false, true));
  anime->addSource("bonfire", trackList[3]);
    SoundSourceProperties *p = new SoundSourceProperties(new Polar3D(1.0, 25, 0.0), true, true);
    p->scale = .45;
    anime->addKeyFrame("bonfire", 0.0, p);

    //anime->addSource("Waterfall", trackList[1]);
    p = new SoundSourceProperties(new Polar3D(1.0, -60, 0.0), true, true);
    p->scale = 0.2;
    anime->addKeyFrame("Waterfall", 0.0,  p);


    //anime->setStartTime("Narrator", 5.0);
    //anime->setStartTime("Narrator2", 0.0);

    //anime->test_KeyFrames("Narrator");
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
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
        &stream,
        NULL, /* no input */
        &outputParameters,
        44100,
        64,//paFramesPerBufferUnspecified, //	FRAMES_PER_BUFFER,//22050/8,
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
    const int fs = (const int)framesPerBuffer;
    //printf("%d\n", fs);
    buffer[0] = new double[fs];
    buffer[1] = new double[fs];
    double **newOverflow = new double*[2];
    newOverflow[0] = new double[199];
    newOverflow[1] = new double[199];

    anime->getBuffer(buffer, newOverflow, timingCounter, framesPerBuffer);

    double outL;// = buffer[0][i] + overflow[0][i];
    double outR;// = buffer[1][i] + overflow[1][i];
    for( i=0; i<framesPerBuffer; i++ )
    {
        if(i < 200 and overflow != NULL){
           outL = buffer[0][i] + overflow[0][i];
           outR = buffer[1][i] + overflow[1][i];
          if(outL > 1.0){
            outL = 1.0;
          }
          if(outR > 1.0){
            outR = 1.0;
          }

          *out++ = outL;//buffer[0][i] + overflow[0][i];
          *out++ = outR;//buffer[1][i] + overflow[1][i];
        } else{
          *out++ = buffer[0][i];
          *out++ = buffer[1][i];
        }
    }

	//this should make audio truer for frames smaller than 200
	//it might not though, need to go through math
	for(i = framesPerBuffer; i < 199 && overflow != NULL; i++){
		newOverflow[0][i] += overflow[0][i];
		newOverflow[1][i] += overflow[1][i];

	}
    overflow = newOverflow;

    timingCounter = timingCounter + framesPerBuffer;
    printf("%d\n", framesPerBuffer);
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
