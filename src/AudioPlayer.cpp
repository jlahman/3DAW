/*
* Skeleton Example code was taken from paex_sine_c++.cpp
  TODO: should this be purely an audio player? Take out all data processing and put that in a seperate class?
        How to get the final buffer data to the audio player?
        If not realtime could have a function to change pointer to data? would realtime make be any different?
 */
#include<iostream>
#include<algorithm>
#include"AudioPlayer.h"

AudioPlayer::AudioPlayer()
{
    /* initialise variables */
    timingCounter = 0;
	buffer.push_back(0.0);
	buffer.push_back(0.0);

}

bool AudioPlayer::open(PaDeviceIndex indexx)
{
	printf("helllo\n");

    PaStreamParameters outputParameters;
printf("helllo\n");
    outputParameters.device = indexx;
    if (outputParameters.device == paNoDevice) {
        return false;
    }
printf("helllo\n");
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paFloat32;  //paInt16; /* 32 bit float output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
printf("helllo\n");
    PaError err = Pa_OpenStream(
        &stream,
        NULL, /* no input */
        &outputParameters,
        44100,
        paFramesPerBufferUnspecified,//FRAMES_PER_BUFFER,//22050/8,
        paNoFlag,
        &AudioPlayer::paCallback,
        this            /* Using 'this' for userData so we can cast to AudioProcessor* in paCallback method */
        );
printf("helllo\n");
    if (err != paNoError)
    {
        /* Failed to open stream to device !!! */
        return false;
    }
printf("helllo\n");
    err = Pa_SetStreamFinishedCallback( stream, &AudioPlayer::paStreamFinished );

    if (err != paNoError)
    {
        Pa_CloseStream( stream );
        stream = 0;

        return false;
    }
printf("helllo\n");
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
	printf("PaCallBack\n");
	bufferLocked = true;
    unsigned long i;
    float* out = (float *) outputBuffer;
	for(int i = 0; i < framesPerBuffer; i++){
		if(!buffer.empty()){
			*out++ = buffer.front();
			buffer.pop_front();
			*out++ = buffer.front();
			buffer.pop_front();
		} else {
			*out++ = 0.0;
			*out++ = 0.0;
			printf("No more data in buffer at i = \t%d", i);
		}
	}

	bufferLocked = false;

	/*

    timingCounter = timingCounter + framesPerBuffer;
    //printf("%d\n", framesPerBuffer);
   // index = index + framesPerBuffer;
   /* Prevent unused variable warnings. */
    (void) timeInfo;
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

int AudioPlayer::buffer_enque(std::vector<double> * data){
	std::copy(data->begin(), data->end(), buffer2.end());
	updateBuffer(0);

	return 0;
}

int AudioPlayer::buffer_enque(double * data, int length){
	//TODO: probably not effecient, look for something similar to the vector copy
	printf("rwarxrdlk;ajsdf\n" );
	for(int i = 0; i < length; i++){
		printf("%d\n",i );

		buffer2.push_back(data[i]);
	}
	updateBuffer(0);

	return 0;
}

int AudioPlayer::buffer_clear(){
	bufferNeedsToClear = true;
	return 0;
}


int AudioPlayer::updateBuffer(int removeLength){
	return 0;
	if(!bufferLocked && bufferNeedsToClear){
		buffer.clear();
		return 0;
	} else {
		if(bufferLocked){
			printf("Buffer is Still Locked!\n");
			return 0;
		} else {			printf("DEBUG: Before copy!\n");
			if(true){printf("DONEON: Before copy!\n");
				buffer.push_back(buffer2.front());printf("DONEON: AFTER copy!\n");
			} else
				std::copy(buffer2.begin(), buffer2.end(), buffer.end());
			printf("RAWR: Before copy!\n");
			return 0;
		}
	}
	return -1;
}
