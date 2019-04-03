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
          trackList.push_back(new Tract("../data/Bee.mp3", "Bee", -90.0));
          trackList.push_back(new Tract("../data/test.wav", "Waterfall", 45.0));

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

        double bufferLeft[framesPerBuffer];
        double bufferRight[framesPerBuffer];
        //TODO: this has more parameters than i'd like, need to clean it up when moving it to a different class
        processFrame(bufferLeft, bufferRight, timingCounter, framesPerBuffer, trackList);

        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = bufferLeft[i];
            *out++ = bufferRight[i];
        }

        //Just to keep circling while animation player is WIP
        double newAzi = trackList[0]->getAzimuth() + 180 + 5;
        newAzi = fmod(newAzi, 360);
        newAzi -= 180;
        trackList[0]->setAzimuth(newAzi);

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

    void processFrame(double * bufL, double * bufR, int counter, int framesPerBuffer, std::vector<Tract*> tracks){
      double * mDataChunk = new double[framesPerBuffer-199];
      double * audioData;// = *trackList[0]->getData();

      convDataSize = framesPerBuffer;
      cData = new double[convDataSize];
      cDataR = new double[convDataSize];

      for(int i =0; i< framesPerBuffer; i++){
        bufL[i] = 0;
        bufR[i] = 0;
      }

      for(std::vector<Tract*>::iterator track = tracks.begin(); track != tracks.end(); track++){
        audioData = *(*track)->getData();
        for(int i = 0; i < framesPerBuffer - 199; i++){

          if((*track)->isLooping()){
            mDataChunk[i] = audioData[(i+counter)%(*track)->getLength()];
          }
          else {
            if(i + counter >= (*track)->getLength()){
              mDataChunk[i] = 0;
            } else{
              mDataChunk[i] = audioData[i+counter];
            }
          }
        }
        int aziIndex = hrir->getIndices((*track)->getAzimuth(), 0)[0];
        int eleIndex = hrir->getIndices((*track)->getAzimuth(), 0)[1];
        convolve(mDataChunk, framesPerBuffer - 199, hrir->hrir_l[aziIndex][eleIndex], 200, cData);
        convolve(mDataChunk, framesPerBuffer - 199, hrir->hrir_r[aziIndex][eleIndex], 200, cDataR);

        for(int i =0; i< framesPerBuffer; i++){
          bufL[i] += cData[i];
          bufR[i] += cDataR[i];
        }

      }
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
