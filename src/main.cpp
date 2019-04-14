#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>


#include"Track.h"
#include"util.h"
#include"AudioPlayer.h"

#include"AnimationPlayer.h"

#define NUM_SECONDS   (16)
std::vector<Track*> trackList;

int main(int argc,  char * argv[])
{

    PaError err;
    AudioPlayer ap = AudioPlayer();
    AnimationPlayer * anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_058/hrir_final.mat");

    trackList.push_back(new Track("../data/souldfire.wav"));

	anime->addSource("Narrator2", trackList[0]);
	anime->test_KeyFrames("Narrator2");


	const int framesPerAnimationStep = 4096;
	int frameCount = 0;
	bool animePlay = true;
	const int frameStop = 44100 * 5 ;
	const int audioOutSize = 2*frameStop;

	double ** audioOut = new double*[2];
	audioOut[0] = new double[framesPerAnimationStep];
	audioOut[1] = new double[framesPerAnimationStep];

	double ** overflow = new double*[2];
	overflow[0] = new double[199];
	overflow[1] = new double[199];

	double *audioOutInterlaced = new double[audioOutSize];

	//anime->getBuffer(audioOut, overflow, frameCount, (const int)framesPerAnimationStep);

	/*for(int i = 0; i < frameStop; i++){
		//printf("%E\n", audioOut[0][i]);
		audioOutInterlaced[2*i] = audioOut[0][i];
		audioOutInterlaced[2*i + 1] = audioOut[1][i];
		//frameCount += framesPerAnimationStep;
		//printf("%d\n", i);
	}*/

	printf("%d\n", frameStop*2-1);


	while(ap.getBufferMax() > ap.buffer_size() && frameCount < frameStop){
		anime->getBuffer(audioOut, overflow, frameCount, (const int)framesPerAnimationStep);

		for(int i = 0; i < framesPerAnimationStep; i++){
			//printf("%E\n", audioOut[0][i]);
			audioOutInterlaced[2*i+frameCount*2] += audioOut[0][i];
			audioOutInterlaced[2*i+frameCount*2+ 1] += audioOut[1][i];
			//printf("%d\n", i);
		}
		for(int i = framesPerAnimationStep; i < framesPerAnimationStep+199 && i< audioOutSize; i++){
			//printf("%E\n", audioOut[0][i]);
			audioOutInterlaced[2*i+frameCount*2] = overflow[0][i-framesPerAnimationStep];
			audioOutInterlaced[2*i+frameCount*2+ 1] = overflow[1][i-framesPerAnimationStep];
			//printf("%d\n", i);
		}
		frameCount += framesPerAnimationStep;


	}
	ap.buffer_enque(audioOutInterlaced, audioOutSize);
	ap.updateBuffer(0);//(audioOutInterlaced, audioOutSize);




	printf("PortAudio Test\n");

	//ap.buffer_enque(audioOutInterlaced, audioOutSize);




    printf("PortAudio Test\n");
	err = Pa_Initialize();
	if( err != paNoError ) goto error;

	if (ap.open(Pa_GetDefaultOutputDevice()))
	{
		std::string input;
		bool done = false;
		bool pause = true;

		while(!done){

			//handle input
			std::cin >> input;
			if(input == "pause"){
				pause = !pause;
			} else if(input == "r") {
				ap.restart();
			} else if(input == "q") {
				done = true;
			} else if(input == "add") {
				std::cin >> input;
				if(input == "track"){
					std::cin >> input;
					trackList.push_back(new Track(input));
					printf("%d\n", trackList.size() );
				} else if(input == "source") {
					std::cin >> input;
					anime->addSource(trackList[input.at(0)-'0']->name ,trackList[input.at(0)-'0']);
					printf("%d\n", trackList.size() );
				}
			} else if(input == "recompute") {
				ap.stop();
				ap.buffer_clear();
				ap.updateBuffer(0);
				frameCount = 0;
				while(ap.getBufferMax() > ap.buffer_size() && frameCount < frameStop){
					anime->getBuffer(audioOut, overflow, frameCount, (const int)framesPerAnimationStep);

					for(int i = 0; i < framesPerAnimationStep; i++){
						//printf("%E\n", audioOut[0][i]);
						audioOutInterlaced[2*i+frameCount*2] += audioOut[0][i];
						audioOutInterlaced[2*i+frameCount*2+ 1] += audioOut[1][i];
						//printf("%d\n", i);
					}
					for(int i = framesPerAnimationStep; i < framesPerAnimationStep+199 && i< audioOutSize; i++){
						//printf("%E\n", audioOut[0][i]);
						audioOutInterlaced[2*i+frameCount*2] = overflow[0][i-framesPerAnimationStep];
						audioOutInterlaced[2*i+frameCount*2+ 1] = overflow[1][i-framesPerAnimationStep];
						//printf("%d\n", i);
					}
					frameCount += framesPerAnimationStep;


				}
				ap.buffer_enque(audioOutInterlaced, audioOutSize);
				ap.updateBuffer(0);//(audioOutInterlaced, audioOutSize);

			}

			if (!pause)
			{		ap.start();

				//printf("Play\n" );
			} else {
				//printf("Pause\n" );
				ap.stop();
			}
		}
		ap.close();
	}

	Pa_Terminate();
	printf("Test finished.\n");

	delete[] audioOut[0];
	delete[] audioOut[1];
	delete[] audioOut;
	printf("PortAudio Test\n");

	delete[] overflow[1];	printf("PortAudio Test\n");
	overflow[1] = NULL;
	delete[] overflow[0];	printf("PortAudio Test\n");
	overflow[0] = NULL;
	delete[] overflow;	printf("PortAudio Test\n");
	overflow = NULL;

	delete[] audioOutInterlaced;


	return err;

	error:
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return err;

	//return 0;
}
