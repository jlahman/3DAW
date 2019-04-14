#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>


#include"Track.h"
#include"util.h"
#include"AnimationPlayer.h"
#include"AudioPlayer.h"

#define NUM_SECONDS   (16)
std::vector<Track*> trackList;

int main(int argc,  char * argv[])
{

    PaError err;
    //AudioPlayer* ap;
    AnimationPlayer * anime;
	//ap = new AudioPlayer();
	anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_058/hrir_final.mat");

    trackList.push_back(new Track("../data/test.wav"));


	anime->addSource("Narrator2", trackList[0]);


	const int framesPerAnimationStep = 4096;
	int frameCount = 0;
	bool animePlay = true;
	const int frameStop = 441 * 1;
	const int audioOutSize = 2*frameStop;

	double ** audioOut = new double*[2];
	audioOut[0] = new double[frameStop];
	audioOut[1] = new double[frameStop];
	double ** overflow = new double*[2];
	overflow[0] = new double[200-1];
	overflow[1]= new double[200-1];
	double *audioOutInterlaced = new double[audioOutSize];
	
	anime->getBuffer(audioOut, overflow, frameCount, (const int)frameStop);
	for(int i = 0; i < frameStop; i++){
		//printf("%E\n", audioOut[0][i]);
		audioOutInterlaced[2*i] = audioOut[0][i];
		audioOutInterlaced[2*i + 1] = audioOut[1][i];
		//frameCount += framesPerAnimationStep;
		//printf("%d\n", i);
	}

	printf("%d\n", frameStop*2-1);


	delete[] audioOut[0];
	delete[] audioOut[1];
	delete[] audioOut;
	printf("PortAudio Test\n");

	delete[] overflow[0];	printf("PortAudio Test\n");

	delete[] overflow[1];	printf("PortAudio Test\n");

	delete[] overflow;	printf("PortAudio Test\n");


	printf("PortAudio Test\n");

	//ap->buffer_enque(audioOutInterlaced, audioOutSize);


	//delete[] audioOutInterlaced;


    printf("PortAudio Test\n");


	return 0;
}
