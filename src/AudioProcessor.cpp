
/*
 * $Id: paex_sine.c 1752 2011-09-08 03:21:55Z philburk $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
* Skeleton Example code was taken from paex_sine_c++.cpp
 */
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <cstring>
#include<matio.h>
#include<portaudio.h>

#include"HRIR_Data.h"

#define NUM_SECONDS   (5)


#define FRAMES_PER_BUFFER  (64)



bool isBigEndian()
{
    int a = 1;
    return !((char*)&a)[0];
}

int convertToInt(char* buffer, int len)
{
    int a = 0;
    if (!isBigEndian())
        for (int i = 0; i<len; i++)
            ((char*)&a)[i] = buffer[i];
    else
        for (int i = 0; i<len; i++)
            ((char*)&a)[3 - i] = buffer[i];
    return a;
}


char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size)
{
    char buffer[4];
    std::ifstream in(fn, std::ios::binary);
    in.read(buffer, 4);
    if (strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cout << "this is not a valid WAVE file" << std::endl;
        return NULL;
    }
    in.read(buffer, 4);
    in.read(buffer, 4);      //WAVE
    in.read(buffer, 4);      //fmt
    in.read(buffer, 4);      //16

    in.read(buffer, 2);      //1
    in.read(buffer, 2);
    chan = convertToInt(buffer, 2);
    in.read(buffer, 4);
    samplerate = convertToInt(buffer, 4);
    in.read(buffer, 4);
    in.read(buffer, 2);
    in.read(buffer, 2);
    bps = convertToInt(buffer, 2);
    in.read(buffer, 4);      //data
    in.read(buffer, 4);
    size = convertToInt(buffer, 4);
    char* data = new char[size];
    in.read(data, size);
    return data;
}

void convolve( int16_t Signal[/* SignalLen */], uint32_t SignalLen, double Kernel[/* KernelLen */], uint32_t KernelLen, double * output)
{
	//printf("asdf\n");
  uint32_t n;
  //printf("asdf\n");
	//std::cout << SignalLen + KernelLen - 1 << "   h||||||||||||||h   "<< SignalLen << std::endl;
  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    uint32_t kmin, kmax, k;

    output[n] = 0;

    kmin = (n >= SignalLen - 1) ? n - (SignalLen - 1) : 0;
    kmax = (n < KernelLen - 1) ? n : KernelLen - 1;
	//std::cout<< n<< "\t" << kmin << "\t" << kmax <<std::endl;
    for (k = kmin; k <= kmax; k++)
    { //if(n == 110916) std::cout << "\tk = " << k <<  "\toutput[60414] = " << output[n]<<std::endl;
      output[n] += Signal[n - k] * Kernel[k];
    }

    output[n] = output[n]/32768.0;
    if(output[n] > 1.0){
      output[n] = 1.0;

    }
    else if( output[n] < -1.0) {
      output[n] = -1.0;
    }
   // if(n % 100 == 0)
    	//std::cout<< output[n] << std::endl;
    //output[n] = (uint16_t)((double)output[n]/32768.0);
  }
}

class AudioProcessor
{
public:
      AudioProcessor( int azimuthIndex, int elevationIndex)
      {
          /* initialise variables */
          index = 0;
          aziIndex = 0;
          timingCounter = 0;
        	data = loadWAV("../data/test.wav", channel, sampleRate, bps, size);
  		    mData = new int16_t[size/2];

          hrir = new HRIR_Data("../data/CIPIC_hrtf_database/standard_hrir_database/subject_033/hrir_final.mat",0,8);

      		convDataSize = size/2 + 200 -1;
      		cData = new double[convDataSize];
      		cDataR = new double[convDataSize];
      		for(int i = 0; i< size/2;i++){
      		//std::cout << i <<"\tmdata = "<<mData[i]<< "\tmdata_after" << std::endl;
      			char temp[2];
      			temp[0] = data[i*2];
      			temp[1] = data[i*2+1];
      			mData[i] = (int16_t)convertToInt(temp,2);// 32768.0; std::cout << mData[i] << std::endl;

      		}

      		convolve(mData, size/2, hrir->hrir_l[4][8], 200, cData);
      		finalData = new double[convDataSize*2];

      		convolve(mData, size/2, hrir->hrir_r[4][8], 200, cDataR);
      		finalDataR = new double[convDataSize*2];

      		for(int i = 0; i<convDataSize; i = i+1){
      			/*uint16_t temph = (cData[i] & 0xff00);
      			uint16_t templ = (cData[i] & 0x00ff);
      			templ = templ << 8;
      			templ = templ | ((temph >> 8 ) & 0x00ff);*/
      			finalData[i*2] = cData[i];
      			finalData[i*2 + 1] = cDataR[i];
            //std::cout << i << "\t" << cData[i] << std::endl;
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
        outputParameters.sampleFormat = paFloat32;//paInt16; /* 16 bit int output */
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

        printf("%d\t%d\t%d\t%d\n", aziIndex, timingCounter, index, size/2);
        convDataSize = framesPerBuffer + 200 -1;
        cData = new double[convDataSize];
        cDataR = new double[convDataSize];

        int16_t * mDataChunk = new int16_t[framesPerBuffer-199];
        for(int i = 0; i< framesPerBuffer - 199; i++){
          if(i+timingCounter < size/2){
            mDataChunk[i] = mData[i+timingCounter];
          }
          else{
            mDataChunk[i] = 0;
          }
        }

        convolve(mDataChunk, framesPerBuffer - 199, hrir->hrir_l[aziIndex][8], 200, cData);
        convolve(mDataChunk, framesPerBuffer - 199, hrir->hrir_r[aziIndex][8], 200, cDataR);

      //  convolve(mData, size/2, hrir->hrir_r[aziIndex][8], 200, cDataR);


        for( i=0; i<framesPerBuffer; i++ )
        {
            *out++ = cData[i] ;   /* left - distorted */
            *out++ = cDataR[i] ;         /* right - clean */
            index = index+1;

        }

        aziIndex = aziIndex + 1;
        if(aziIndex == 25){
          aziIndex = 0;
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
        printf( "Stream Completed: %s\n", message );
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
    int16_t *mData;
    double* cData;
    double* cDataR;
    double* finalData;
    double* finalDataR;
   	size_t convDataSize;
    int aziIndex;
    int timingCounter;
    char message[20];

};


/*******************************************************************/
int main( int argc,  char*  argv[]);
int main(int argc,  char * argv[])
{
    PaError err;
    AudioProcessor* ap;

  /*  if( argc == 3) {
      printf("halkdjsflkasjdflkjsdf\n");
      int arg1, arg2;
      arg1 = atoi(argv[1]);
      arg2 = atoi(argv[2]);
       ap = new AudioProcessor(arg1, arg2);
    }
    else*/
         ap = new AudioProcessor(24, 8);



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
