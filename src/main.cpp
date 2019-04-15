#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <sstream>

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
bool pause = true;

const int frameStop = 44100 * 50 ;

double ** audioOut = NULL;

double ** overflow = NULL;

void foo(){
	while(!done){
		while(ap.getBufferMax() > ap.buffer_size() && frameCount < frameStop && animePlay){
			int writeLength = (ap.getBufferMax() - ap.buffer_size())/2;
			//printf("DEBUG: 1 \t%d \n", frameCount);

			if(writeLength >= framesPerAnimationStep){
				writeLength = framesPerAnimationStep;
			}
			//printf("DEBUG: 2 \t%d \n", frameCount);

			audioOut  = new double*[2];
			audioOut[0] = new double[writeLength];
			audioOut[1] = new double[writeLength];

			//printf("DEBUG: 3 \t%d \n", frameCount);

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


			//printf("DEBUG: 7 \t%d \n", frameCount);

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

void handle_input();

int main(int argc,  char * argv[])
{

    PaError err;

    trackList.push_back(new Track("../data/souldfire.wav"));
	anime->addSource("Narrator2", trackList[0]);
	anime->test_KeyFrames("Narrator2");

    printf("PortAudio Test\n");
	err = Pa_Initialize();
	if( err != paNoError ) goto error;

	if (ap.open(Pa_GetDefaultOutputDevice()))
	{
		std::string input;
		done = false;
		//bool pause = true;
		std::thread(foo).detach();
		//nice.detach();
		std::cin >> input;
		//Pa_Sleep(1000);

		ap.start();
		//done = true;
		while(!done){
			handle_input();


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
//from fluentcpp: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
};

void handle_input(){
	std::string input;
	std::getline(std::cin, input);
	char delim = ' ';
	std::vector<std::string>line = split((const std::string) input, delim);
	if(line.size() == 0) return;
	if(line[0]  == "p"){
		pause = !pause;
		if (!pause)
		{
			ap.start();
			animePlay = true;
			//std::cout << "Playing\r" << std::flush;
			//printf("Play\n" );
		} else {
			ap.stop();
			//printf("Pause\n" );
			//std::cout << "Paused\r"<< std::flush;

			animePlay = false;
		}
	} else if(line[0] == "r") {
		frameCount = 0;
	} else if(line[0] == "q") {
		done = true;
	}

	if(line[0] ==  "import"){
		if(line[1] == "-file" || line[1] == "-f"){
			Track * track = new Track(line[2]);
			trackList.push_back(track);
			std::cout << "\nAdded file \"" << line[2] << "\" successfully." << std::endl;
		}
	} else if(line[0] ==  "export") {
		   //export
		   std::cout << "\nExporting Not Yet Supported, Stay Tuned." << std::endl;
	} else if (line[0] ==  "add"){
		if(line[1] == "-source" || line[1] == "-s"){
			if(line.size() == 4){
				anime->addSource(line[2], trackList.at(std::stoi(line[3], nullptr, 10)));
				std::cout << "\nAdded new Source \"" << line[2] << "\t"<< line[3] << "\" successfully." << std::endl;
			} else {
				std::cout << "\nERROR adding new Source: wrong number of arguments! expected: 4, got: " << line.size() << std::endl;
			}
		}
		else if(line[1] == "-keyframe" || line[1] == "-k"){
			//anime->addSource(new SoundSource(line[2], std::stoi(line[3], NULL, 10)));
			std::cout << "Added new KeyFrame at time \"" << line[2] << "\" NOT IMPLEMENTED." << std::endl;
		}
	}  else if(line[0] ==  "list") {
		if(line[1] == "-source" || line[1] == "-s"){
			double time = std::stoi(line[2], nullptr, 10)/1000.0;
			std::cout << "\nSources in Composition: @ time (seconds) = " << time << std::endl;
			for(int i = 0; i < anime->getSources(time).size(); i ++){
				std::cout << "\t|====> " << anime->getSources(time).at(i)->getName() << "\t: Index: \t" << i << " Length (s):\t" << anime->getSources(time).at(i)->getLength()/44100.0/2.0 << std::endl;
			}
			std::cout << "\t|=======I=====================================================|\n";
		} else if(line[1] == "-keyframe" || line[1] == "-k"){
		}
	} else {
		std::cout << "Input Not Recognized, Perhaps not yet implemented?"<< std::endl;
	}
};
