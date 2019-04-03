/*
* Skeleton Example code was taken from paex_sine_c++.cpp
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include<matio.h>
#include<portaudio.h>


#include"HRIR_Data.h"
#include"Tract.h"


#define NUM_SECONDS   (10)


#define FRAMES_PER_BUFFER  (64)

//TODO: move this to a util file
void convolve( double* Signal, uint32_t SignalLen, double* Kernel, uint32_t KernelLen, double * output)
{
  uint32_t n;
  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    uint32_t kmin, kmax, k;

    output[n] = 0;

    kmin = (n >= SignalLen - 1) ? n - (SignalLen - 1) : 0;
    kmax = (n < KernelLen - 1) ? n : KernelLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      output[n] += Signal[n - k] * Kernel[k];
    }

    output[n] = output[n];
    if(output[n] > 1.0){
      output[n] = 1.0;

    }
    else if( output[n] < -1.0) {
      output[n] = -1.0;
    }
  }
}

class AudioProcessor
{
public:
      AudioProcessor()
      {
          /* initialise variables */
          index = 0;
          aziIndex = 0;
          eleIndex = 8;
          inc = 1;
          timingCounter = 0;
          trackList.push_back(new Tract("../data/Bee.mp3", "Bee"));
          trackList.push_back(new Tract("../data/test.wav", "Waterfall"));

          //tract2 = new Tract("../data/Bee.mp3", "Bees");


          hrir = new HRIR_Data("../data/CIPIC_hrtf_database/standard_hrir_database/subject_162/hrir_final.mat");

      		convDataSize = trackList[1]->getLength() + 200 -1;
      		cData = new double[convDataSize];
      		cDataR = new double[convDataSize];


      		convolve(*trackList[1]->getData(), trackList[1]->getLength(), hrir->hrir_l[16][8], 200, cData);
      		finalData = new double[convDataSize];

      		convolve(*trackList[1]->getData(), trackList[1]->getLength(), hrir->hrir_r[16][8], 200, cDataR);
      		finalDataR = new double[convDataSize];

      		for(int i = 0; i<convDataSize; i = i+1){
      			finalData[i] = cData[i];
      			finalDataR[i] = cDataR[i];
      		}
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
            22050/8,
            paNoFlag,      /* we won't output out of range samples so don't bother clipping them */
            &AudioProcessor::paCallback,
            this            /* Using 'this' for userData so we can cast to Sine* in paCallback method */
            );

        if (err != paNoError)
        {
            /* Failed to open stream to device !!! */
            return false;
        }

        err = Pa_SetStreamFinishedCallback( stream, &AudioProcessor::paStreamFinished );

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

        //timingCounter = index;

      //  printf("%d\t%d\t%d\t%d\n", aziIndex, timingCounter, index, test->getLength());
        convDataSize = framesPerBuffer + 200 -1;
        cData = new double[convDataSize];
        cDataR = new double[convDataSize];

        double * mDataChunk = new double[framesPerBuffer-199];
        double * audioData = *trackList[0]->getData();

        for(int i = 0; i< framesPerBuffer - 199; i++){
          if(i+(timingCounter%trackList[0]->getLength()) < trackList[0]->getLength()){
            mDataChunk[i] = audioData[i+(timingCounter%trackList[0]->getLength())];
          }
          else{
            mDataChunk[i] = 0;
          }
        }
        convolve(mDataChunk, framesPerBuffer - 199, hrir->hrir_l[aziIndex][eleIndex], 200, cData);
        convolve(mDataChunk, framesPerBuffer - 199, hrir->hrir_r[aziIndex][eleIndex], 200, cDataR);



        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = cData[i] + finalData[i+(timingCounter%trackList[1]->getLength())] ;   /* left - distorted */
            *out++ = cDataR[i] + finalDataR[i+(timingCounter%trackList[1]->getLength())];         /* right - clean */
            index = index+1;

        }

        aziIndex = aziIndex + inc;
        if(aziIndex == 25){
        	aziIndex = 24;
			    inc = -1;
          eleIndex = 40;
        }
        if(aziIndex == -1){
			    inc = 1;
			    aziIndex = 0;
          eleIndex = 8;
        }



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


        return ((AudioProcessor*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
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
        return ((AudioProcessor*)userData)->paStreamFinishedMethod();
    }

    PaStream *stream;

    int channel, sampleRate, bps, size;
    HRIR_Data *hrir;
    char* data;
    uint32_t index;
    double* cData;
    double* cDataR;
    double* finalData;
    double* finalDataR;
   	size_t convDataSize;
    int aziIndex, eleIndex, inc;
    int timingCounter;
    char message[20];
    Tract * test;
    Tract * tract2;
    std::vector<Tract*> trackList;

};


/*******************************************************************/
int main( int argc,  char*  argv[]);
int main(int argc,  char * argv[])
{
    PaError err;
    AudioProcessor* ap;

   ap = new AudioProcessor();

    printf("PortAudio Test\n");

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    if (ap->open(Pa_GetDefaultOutputDevice()))
    {
        if (ap->start())
        {
            printf("Play for %d seconds.\n", NUM_SECONDS );        printf("I wub hate this\n" );

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
