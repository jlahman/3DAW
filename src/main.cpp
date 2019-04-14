#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <thread>

#include"Track.h"
#include"util.h"
#include"AudioPlayer.h"

#include"AnimationPlayer.h"

#define NUM_SECONDS   (16)
std::vector<Track*> trackList;
AudioPlayer ap = AudioPlayer();
AnimationPlayer * anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_058/hrir_final.mat");
const int framesPerAnimationStep = 1024;
int frameCount = 0;
bool animePlay = true;
bool done = false;

const int frameStop = 44100 * 50 ;

double ** audioOut = NULL;

double ** overflow = NULL;

void foo(){
	while(!done){
		while(ap.getBufferMax() > ap.buffer_size() && frameCount < frameStop && animePlay){
			int writeLength = (ap.getBufferMax() - ap.buffer_size())/2;
			printf("DEBUG: 1 \t%d \n", frameCount);

			if(writeLength >= framesPerAnimationStep){
				writeLength = framesPerAnimationStep;
			}
		//	printf("DEBUG: 2 \t%d \n", frameCount);

			audioOut  = new double*[2];
			audioOut[0] = new double[writeLength];
			audioOut[1] = new double[writeLength];

		//	printf("DEBUG: 3 \t%d \n", frameCount);

			double ** newOverflow = new double*[2];
			newOverflow[0] = new double[199];
			newOverflow[1] = new double[199];

			const int audioOutSize = 2*writeLength;
			double *audioOutInterlaced = new double[audioOutSize];

			//printf("DEBUG: 4 \t%d \n", frameCount);

			anime->getBuffer(audioOut, newOverflow, frameCount, (const int)writeLength);
			//printf("DEBUG: 5 \t%d \n", frameCount);

			for(int i = 0; i < writeLength; i++){
				//printf("%E\n", audioOut[0][i]);
				if(overflow != NULL && i < 199){
					audioOutInterlaced[2*i] = audioOut[0][i] + overflow[0][i];
					audioOutInterlaced[2*i+ 1] = audioOut[1][i] + overflow[1][i];
				} else {
					audioOutInterlaced[2*i] = audioOut[0][i];
					audioOutInterlaced[2*i+ 1] = audioOut[1][i];
				}
				//fprintf(stderr, "%f\n", audioOutInterlaced[i*2] );
			}
			//printf("DEBUG: 6 \t%d \n", frameCount);
			for(int i = writeLength; i < 199 && overflow != NULL; i++){
				newOverflow[0][i-writeLength] += overflow[0][i];
				newOverflow[1][i-writeLength] += overflow[1][i];

			}
			if(overflow != NULL){
				delete[] overflow[0];
				delete[] overflow[1];
				delete[] overflow;
			}

			if(audioOut != NULL){
				delete[] audioOut[0];
				delete[] audioOut[1];
				delete[] audioOut;
			}


			printf("DEBUG: 7 \t%d \n", frameCount);

			overflow = newOverflow;
			frameCount += writeLength;
			ap.buffer_enque(audioOutInterlaced, audioOutSize);
			//printf("%d \n", frameCount/framesPerAnimationStep);
			if(audioOutInterlaced != NULL){
				delete[] audioOutInterlaced;
			}
		}
	}
}

int main(int argc,  char * argv[])
{

    PaError err;

    trackList.push_back(new Track("../data/souldfire.wav"));
	anime->addSource("Narrator2", trackList[0]);
	anime->test_KeyFrames("Narrator2");

	//std::thread nice(foo);

	//ap.buffer_enque(audioOutInterlaced, audioOutSize);
	//ap.updateBuffer(0);//(audioOutInterlaced, audioOutSize);




	printf("PortAudio Test\n");

	//ap.buffer_enque(audioOutInterlaced, audioOutSize);




    printf("PortAudio Test\n");
	err = Pa_Initialize();
	if( err != paNoError ) goto error;

	if (ap.open(Pa_GetDefaultOutputDevice()))
	{
		std::string input;
		done = false;
		bool pause = true;
		std::thread(foo).detach();
		//nice.detach();
		std::cin >> input;
		Pa_Sleep(1000);

		ap.start();
		//done = true;
		while(!done){
			//handle input
			std::cin >> input;
			if(input == "p"){
				pause = !pause;
			} else if(input == "r") {
				frameCount = 0;
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

				frameCount = 0;

			}

			if (!pause)
			{
				ap.start();
				animePlay = true;

				//printf("Play\n" );
			} else {
				ap.stop();
				//printf("Pause\n" );
				animePlay = false;
			}
		}
		ap.close();
		//nice.~thread();
	}

	Pa_Terminate();
	printf("Test finished.\n");

	printf("PortAudio Test\n");

	//delete[] audioOutInterlaced;


	return err;

	error:
		Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
		return err;

	//return 0;
}
