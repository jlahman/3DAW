/*
* Skeleton Example code was taken from paex_sine_c++.cpp*/
#include<iostream>
#include<algorithm>
#include"AudioPlayer.h"

AudioPlayer::AudioPlayer()
{
    /* initialise variables */
    timingCounter = 0;
	bufferMax = 2*44100*1.5;
	bufferSize = 0;// 2*44100*1.5;
	bufferIntern = &bufferSwap1;
	bufferExtern = &bufferSwap2;

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
//updateBuffer(0);
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
	//printf("PaCallBack\n");
    float* out = (float *) outputBuffer;
	printf("BEFORE %s\n", bufferLocked ? "true" : "false");

//	while(bufferLocked){};
//bufferLocked = true;
	//std::vector<double> tempBuffer;
	printf("%s\n", bufferLocked ? "true" : "false");
	//get data for this frame
	int length = bufferIntern->size();
	if(length >= framesPerBuffer*2){
		length = framesPerBuffer*2;
		if(!bufferIntern->empty())
			std::copy(bufferIntern->begin(), (bufferIntern->begin()+length), std::back_inserter(buffer));
	} else {
		if(!bufferIntern->empty())
			std::copy(bufferIntern->begin(), (bufferIntern->begin()+length), std::back_inserter(buffer));

		bufferIntern->clear();

		//Switch buffer used for internal data
		//set lock
		bufferLocked = true;
		//store pointer of intern,
		std::deque<double> * temp = bufferIntern;
		//set pointer of intern to other swap buffer
		if(bufferIntern == &bufferSwap1){
			bufferIntern = &bufferSwap2;
		} else {
			bufferIntern = &bufferSwap1;
		}
		//set extern to stored
		bufferExtern = temp;
		//reset bufferSize
		bufferSize = bufferExtern->size();
		//release lock
		bufferLocked = false;

		//Set remainder of buffer
		if(bufferIntern->size() >= framesPerBuffer*2 - length)
			length = framesPerBuffer*2 - length;
		else
			length  = bufferIntern->size();
		if(!bufferIntern->empty())
			std::copy(bufferIntern->begin(), (bufferIntern->begin()+length), std::back_inserter(buffer));
	}
	//bufferLocked = false;
	//write_index =(write_index + framesPerBuffer)%bufferMax;
	printf("AFTER copy%s\n", bufferLocked ? "true" : "false");

	for(int i = 0; i < framesPerBuffer; i++){
		if(!buffer.empty() && buffer.size() > 1){
			//should only read
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
	printf("%s\n", bufferLocked ? "true" : "false");

	timingCounter = timingCounter + framesPerBuffer;
	printf("%lf\n", timingCounter/44100.0 );
	//while(bufferLocked){};

	//bufferLocked = true;

	for(int i = 0; i < length && !bufferIntern->empty(); i++){
		bufferIntern->pop_front();
	}

	//updateBuffer(0);
	//bufferLocked = false;

	/*


	delete[] buffer[0];
	delete[] buffer[1];
	delete[] buffer;



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
	std::copy(data->begin(), data->end(), bufferExtern->end());
	//updateBuffer(0);

	return 0;
}

int AudioPlayer::buffer_enque(double * data, int length){
	//TODO: probably not effecient, look for something similar to the vector copy
//	if(bufferSize < bufferMax){
printf("BUFFER_ENQUE%s\n", bufferLocked ? "true" : "false");

	while(bufferLocked){}
	printf("%s\n", bufferLocked ? "true" : "false");

	//bufferLocked = true;
		int i = 0;
		for( i = 0; i < length && bufferSize + i < bufferMax; i++){
			/*printf("%d\n",i );
			printf("%E\n", data[i] );
			printf("hewwo\n");
			printf("%lu\n", buffer2.max_size());

*/
//printf("%d%s\n", i, bufferLocked ? "true" : "false");

			bufferExtern->push_back(data[i]);
			//printf("bai\n");

		}
		bufferSize = bufferExtern->size();
		//bufferLocked = false;
	//}
	//updateBuffer(0);

	return 0;
}

int AudioPlayer::buffer_clear(){
	bufferNeedsToClear = true;
	return 0;
}

int AudioPlayer::getBufferMax(){
	return bufferMax;
}

void AudioPlayer::setBufferMax(int max){
	bufferMax = max;
}


int AudioPlayer::buffer_size(){
	return bufferSize;
}


int AudioPlayer::updateBuffer(int removeLength){
	//return 0;
	if(!bufferLocked && bufferNeedsToClear){
		bufferExtern->clear();
		bufferNeedsToClear = false;
		bufferSize = 0;
		return 0;
	} else {
		if(bufferLocked){
			printf("Buffer is Still Locked!\n");
			return 0;
		} else {
		/*	printf("DEBUG: Before copy!\n");
			if(bufferSize < bufferMax){
				if(bufferSize + buffer2.size() <= bufferMax){
					std::copy(buffer2.begin(), buffer2.end(), std::back_inserter(buffer));
					bufferSize += buffer2.size();
				}
				else{
					printf("over max somewhat");//not working
					int distance = bufferMax - buffer.size();
					std::copy(buffer2.begin(), (buffer2.end()), std::back_inserter(buffer));
					bufferSize += buffer2.size();
				}
				buffer2.clear();
			} else
				printf("Max BUFFER SIZE FOOL!\n");*/
			return 0;
		}
	}
	return -1;
}
