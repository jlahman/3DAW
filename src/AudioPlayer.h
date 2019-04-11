/*
* Skeleton Example code was taken from paex_sine_c++.cpp
  TODO: should this be purely an audio player? Take out all data processing and put that in a seperate class?
        How to get the final buffer data to the audio player?
        If not realtime could have a function to change pointer to data? would realtime make be any different?
 */
 #ifndef AUDIOPLAYER_H
 #define AUDIOPLAYER_H


#include <vector>
#include<portaudio.h>


#include"Track.h"
#include"AnimationPlayer.h"
#define FRAMES_PER_BUFFER  (4096)//(2048) //needs to be at least 201 for no errors: 2048 seems to be as low as we can go rn

class AudioPlayer
{
public:
    AudioPlayer();

    bool open(PaDeviceIndex indexx);
    bool  close();
    bool start();
    bool stop();
    bool restart();

private:
    int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags);

    static int paCallback( const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData );

    void paStreamFinishedMethod();
    static void paStreamFinished(void* userData);

    PaStream *stream = NULL;
    AnimationPlayer * anime;
    //HRIR_Data *hrir;
    int timingCounter;
    char message[20];
    double **overflow = NULL;
    std::vector<Track*> trackList;

};
#endif
