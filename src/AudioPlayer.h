#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H
#include <vector>
#include <deque>
#include<portaudio.h>

#include"Track.h"
#include"AnimationPlayer.h"

class AudioPlayer
{
public:
    AudioPlayer();

	//stream operations
    bool open(PaDeviceIndex indexx);
    bool close();
    bool start();
    bool stop();
    bool restart();

	//buffer operations
	int buffer_enque(double * data, int length);
	int buffer_enque(std::vector<double> * data);
	int buffer_clear();
	int getBufferMax();
	void setBufferMax(int max);
	int buffer_size();

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
    int timingCounter;
    char message[20];

	std::deque<double> buffer;
	std::deque<double> bufferSwap1; //TODO: think of a better name for its role
	std::deque<double> bufferSwap2; //TODO: think of a better name for its role
	std::deque<double> *bufferExtern; //TODO: think of a better name for its role
	std::deque<double> *bufferIntern; //TODO: think of a better name for its role

	bool bufferLocked = false;
	bool bufferNeedsToClear = false;

	std::deque<double> dataToEnque;

	int bufferSize;
	int bufferMax;

};
#endif
