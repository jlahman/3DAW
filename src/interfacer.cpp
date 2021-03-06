#include "interfacer.h"
#include <unistd.h>


Interfacer::Interfacer()
{
    //init members
	anime = new AnimationPlayer("../../data/CIPIC_hrtf_database/standard_hrir_database/subject_040/hrir_final.mat");
    loadHRIR();
    std::cout << "started main program. \n" << std::endl;
}

Interfacer::~Interfacer()
{
    //delete members
    std::cout << "Ending main program. \n" << std::endl;
}

int Interfacer::myMain()
{

    PaError err;

    printf("PortAudio Test\n");
    err = Pa_Initialize();
    if (err != paNoError)
        goto error;

    if (ap.open(Pa_GetDefaultOutputDevice()))
    {
        done = false;
        std::thread(&Interfacer::foo, this).detach();

        //file loading: from a project state?
        /*std::filebuf fb;
		if (fb.open ("test_input.txt",std::ios::in))
		{
			std::istream is(&fb);
			while (is){
				handle_input(&is);
			}
			fb.close();
		}*/

        while (!done)
        {
			char cwd[10000];
			if (getcwd(cwd, sizeof(cwd)) != NULL){}
				//printf("Current working dir: %s\n", cwd);
			           //TODO: if no-gui, grab input from cin and send to input
        }
        ap.close();
    }

    Pa_Terminate();
    printf("Exiting Application... finished!\n");

    for (int i = 0; i < trackList.size(); i++)
    {
        delete trackList[i];
    }

    //finally it's done
    //what needed to long ago
    delete anime;

    return err;

error:
    Pa_Terminate();
    fprintf(stderr, "An error occured while using the portaudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    return err;
}

void Interfacer::foo()
{
    while (!done)
    {
        while (ap.getBufferMax() > ap.buffer_size() &&  animePlay)
        {
            int writeLength = (ap.getBufferMax() - ap.buffer_size()) / 2;

            if (writeLength >= framesPerAnimationStep)
            {
                writeLength = framesPerAnimationStep;
            }

            audioOut = new double *[2];
            audioOut[0] = new double[writeLength];
            audioOut[1] = new double[writeLength];

            double **newOverflow = new double *[2];
            newOverflow[0] = new double[199];
            newOverflow[1] = new double[199];

            const int audioOutSize = 2 * writeLength;
            double *audioOutInterlaced = new double[audioOutSize];

            anime->getBuffer(audioOut, newOverflow, frameCount, (const int)writeLength);

            for (int i = 0; i < writeLength; i++)
            {
                if (overflow != NULL && i < 199)
                {
                    audioOutInterlaced[2 * i] = audioOut[0][i] + overflow[0][i];
                    audioOutInterlaced[2 * i + 1] = audioOut[1][i] + overflow[1][i];
                }
                else
                {
                    audioOutInterlaced[2 * i] = audioOut[0][i];
                    audioOutInterlaced[2 * i + 1] = audioOut[1][i];
                }
                if (audioOutInterlaced[2 * i] > 1.0)
                {
                    audioOutInterlaced[2 * i] = 1.0;
                }
                else if (audioOutInterlaced[2 * i] < -1.0)
                {
                    audioOutInterlaced[2 * i] = -1.0;
                }
                if (audioOutInterlaced[2 * i + 1] > 1.0)
                {
                    audioOutInterlaced[2 * i + 1] = 1.0;
                }
                else if (audioOutInterlaced[2 * i + 1] < -1.0)
                {
                    audioOutInterlaced[2 * i + 1] = -1.0;
                }
            }

            //theres still discontinuities when writeLength < 200
            for (int i = writeLength; i < 199 && overflow != NULL; i++)
            {
                newOverflow[0][i - writeLength] += overflow[0][i];
                newOverflow[1][i - writeLength] += overflow[1][i];
            }

            if (overflow != NULL)
            {
                delete[] overflow[0];
                delete[] overflow[1];
                delete[] overflow;
            }

            if (audioOut != NULL)
            {
                delete[] audioOut[0];
                delete[] audioOut[1];
                delete[] audioOut;
            }

            overflow = newOverflow;
            frameCount += writeLength;

            ap.buffer_enque(audioOutInterlaced, audioOutSize);

            if (audioOutInterlaced != NULL)
            {
                delete[] audioOutInterlaced;
            }
        }
    }
}

bool Interfacer::hasTrack(std::string name){
	for(int i = 0; i < trackList.size(); i++){
		if(trackList[i]->filepath == name){
			return true;
		}
	}
	return false;
}

int Interfacer::getTrackIndex(std::string name){
	for(int i = 0; i < trackList.size(); i++){
		if(trackList[i]->filepath == name){
			return i;
		}
	}
	return -1;
}

int Interfacer::getTrackSize(){
	return trackList.size();
}


//from fluentcpp: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
std::vector<std::string> Interfacer::split(const std::string &s, char delimiter)
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

//this is really wack
//but deadlines demand of us
//mediocre hacks
void Interfacer::handle_input(std::string input)
{
    char delim = ' ';
    std::vector<std::string> line = split((const std::string)input, delim);

    if (line.size() == 0)
        return;
    //std::cout << std::string( 100, '\n' );
    if (line[0] == "play")
    {
        pause = false;
        animePlay = true;
        ap.start();
    }
    else if (line[0] == "pause")
    {
        pause = true;
        animePlay = false;
        ap.stop();
    }
    else if (line[0] == "restart" || line[0] == "r")
    {
        frameCount = 0;
    }
    else if (line[0] == "goto" && line.size() == 2)
    {
        try
        {
            double value = std::stod(line[1]);
            frameCount = (int)value * 44100;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error going to selected time!" << std::endl;
        }
    }
    else if (line[0] == "q" || line[0] == "quit")
    {
        done = true;
    }
    //seeing this, need rum
    //more elegant ways exist
    //map strings to enum
    else if (line[0] == "import")
    {
        if (line[1] == "-file" || line[1] == "-f")
        {
            Track *track = new Track(line[2]);
            trackList.push_back(track);
            std::cout << "Added file \"" << line[2] << "\" successfully." << std::endl;
        }
    }
    else if (line[0] == "export")
    {
        if (export_final(line[1]) == 0)
        {
            std::cout << "Exporting Not Yet Supported, Stay Tuned." << std::endl;
        }
        else
        {
            std::cout << "Error exporting to file." << std::endl;
        }
    }
    else if (line[0] == "select")
    {if(anime->getSources().size() < 1)
			return;
        //select
        if (line[1] == "-source" || line[1] == "-s")
        {
            if (line.size() == 3)
            {
                //anime->addSource(line[2], trackList.at(std::stoi(line[3], nullptr, 10)));
                if (anime->hasSource(line[2]))
                {
                    selectedSource = line[2];
                    keyFrameSelected = 0;
                    std::cout << "Selectd Source \"" << selectedSource << "\"" << std::endl;
                }
                else
                {
                    std::cout << "Could not find \"" << line[2] << "\". Current selected source: " << selectedSource << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR selecting source: wrong number of arguments! expected: 3, got: " << line.size() << std::endl;
            }
        }
        else if (line[1] == "-keyframe" || line[1] == "-k")
        {
            //select keyframe
            if (line.size() == 3)
            {
                if (anime->getSource(selectedSource)->keyFrameList.size() > std::stoi(line[2], nullptr, 10) && std::stoi(line[2], nullptr, 10) >= 0)
                {
                    keyFrameSelected = std::stoi(line[2], nullptr, 10);
                    std::cout << "Selectd Key Frame " << line[2] << " of source \"" << selectedSource << "\"" << std::endl;
                }
                else
                {
                    std::cout << "Keyframe " << line[2] << " of Current selected source: " << selectedSource << "does not exist.\n\tIs the right source selected?" << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR selecting keyframe: wrong number of arguments! expected: 3, got: " << line.size() << std::endl;
            }
        }
        else if (line[1] == "-composition" || line[1] == "-c")
        {
            //select compostion
            if (line.size() == 3)
            {
                //anime->addSource(line[2], trackList.at(std::stoi(line[3], nullptr, 10)));
                if (true)
                {
                    selectedComposition = line[2];
                    //keyFrameSelected = 0;
                    std::cout << "Selectd composition \"" << selectedComposition << "\"" << std::endl;
                }
                else
                {
                    std::cout << "Could not find \"" << line[2] << "\". Current selected composition: " << selectedComposition << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR selecting source: wrong number of arguments! expected: 3, got: " << line.size() << std::endl;
            }
        }
    }
    else if (line[0] == "add")
    {


        if (line[1] == "-source" || line[1] == "-s")
        {
            if (line.size() == 4)
            {
                anime->addSource(line[2], trackList.at(std::stoi(line[3], nullptr, 10)));
                std::cout << "Added new Source \"" << line[2] << "\t" << line[3] << "\" successfully." << std::endl;
            }
            else
            {
                std::cout << "ERROR adding new Source: wrong number of arguments! expected: 4, got: " << line.size() << std::endl;
            }
        }
        else if (line[1] == "-keyframe" || line[1] == "-k")
        {if(anime->getSources().size() < 1)
			return;
            anime->addKeyFrame(selectedSource, std::stod(line[2]), new SoundSourceProperties(new Polar3D(1.0, 0.0, 0.0), true, true));
            std::cout << "Added new KeyFrame at time \"" << line[2] << "\" successfully." << std::endl;
        }
    }
    else if (line[0] == "list")
    {
		if(anime->getSources().size() < 1)
			return;

        if (line[1] == "-source" || line[1] == "-s")
        {
            std::cout << "\nSources in Composition: " << time << std::endl;
            for (int i = 0; i < anime->getSources().size(); i++)
            {
                std::cout << "\t|====> " << anime->getSources().at(i)->getName() << "\t: Index: \t" << i << " Length (s):\t" << anime->getSources().at(i)->getLength() / 44100.0 << std::endl;
            }
            std::cout << "\t|=============================================================|\n";
        }
        else if (line[1] == "-keyframe" || line[1] == "-k")
        {
            int index = 0;
            int end = anime->getSource(selectedSource)->keyFrameList.size();
            if (line.size() == 3)
            {
                index = std::stoi(line[2], nullptr, 10);
                end = index + 1;
            }
            std::cout << "\nKey Frames in Source \"" << selectedSource << "\": " << std::endl;
            for (int i = index; i < end; i++)
            {
                std::cout << "\t|====> : KeyFrame: " << i << "\t Time: " << anime->getSource(selectedSource)->keyFrameList.at(i)->time_s << "\n";
                std::cout << "\t|      |---------> " << SSPNames[1] << ": \t" << anime->getSource(selectedSource)->keyFrameList.at(i)->properties->position->radius << std::endl;
                std::cout << "\t|      |---------> " << SSPNames[2] << ": \t" << anime->getSource(selectedSource)->keyFrameList.at(i)->properties->position->theta << std::endl;
                std::cout << "\t|      |---------> " << SSPNames[3] << ": \t" << anime->getSource(selectedSource)->keyFrameList.at(i)->properties->position->phi << std::endl;

                std::cout << "\t|      |---------> " << SSPNames[4] << ": \t" << anime->getSource(selectedSource)->keyFrameList.at(i)->properties->scale << std::endl;
                std::string temp = "false";
                if (anime->getSource(selectedSource)->keyFrameList.at(i)->properties->isLooping == true)
                    temp = "true";
                std::cout << "\t|      |---------> " << SSPNames[5] << ": \t" << temp << std::endl;
                temp = "false";
                if (anime->getSource(selectedSource)->keyFrameList.at(i)->properties->isVisible == true)
                    temp = "true";
                std::cout << "\t|      |---------> " << SSPNames[6] << ": \t" << temp << std::endl;
            }
            std::cout << "\t|=============================================================|\n";
        }
    }
    else if (line[0] == "remove")
    {
        if (line[1] == "-source" || line[1] == "-s")
        {
            if (line.size() == 3)
            {
                if (anime->hasSource(line[2]))
                {
                    anime->removeSource(line[2]);
                    std::cout << "Removed Source \"" << line[2] << "\" successfully." << std::endl;
                    if (line[2] == selectedSource)
                    {
                        std::cout << "Resetting selected source to first in list if it exists" << std::endl;
                        if (!anime->getSourceAt(0))
                            selectedSource = anime->getSourceAt(0)->source->getName();
                    }
                }
                else
                {
                    std::cout << "Source \"" << line[2] << "\" does not exist. Check for typos?" << std::endl;
                }
            }
            else
            {
                std::cout << "ERROR removing Source: wrong number of arguments! expected: 3, got: " << line.size() << std::endl;
            }
        }
        else if (line[1] == "-keyframe" || line[1] == "-k")
        {
            if (line.size() == 3)
            {
                if (anime->getSource(selectedSource)->keyFrameList.size() > std::stoi(line[2], nullptr, 10) && 0 <= std::stoi(line[2], nullptr, 10))
                {
                    delete anime->getSource(selectedSource)->keyFrameList.at(std::stoi(line[2], nullptr, 10));
                    anime->getSource(selectedSource)->keyFrameList.erase(anime->getSource(selectedSource)->keyFrameList.begin() + std::stoi(line[2], nullptr, 10));
                    std::cout << "Removed KeyFrame " << line[2] << " from source \"" << selectedSource << "\" successfully. \n\tSelected key frame reset to 0!" << std::endl;
                }
                else
                    std::cout << "No keyframe with that index exists!\n";
            }
            else
            {
                std::cout << "ERROR removing Source: wrong number of arguments! expected: 3, got: " << line.size() << std::endl;
            }
        }
    }
    else if (line[0] == "set")
    {
        if (line.size() > 4)
        {
            if (line[2] == "-property" || line[2] == "-p")
            {
                if (line[1] == "-source" || line[1] == "-s")
                {if(anime->getSources().size() < 1)
					return;

                    //TODO: set source property
                    std::string propertyToEdit = line[3];
                    std::string propertyValNew = line[4];
                    set_property_source(propertyToEdit, propertyValNew);
                }
                else if (line[1] == "-keyframe" || line[1] == "-k")
                {if(anime->getSources().size() < 1)
					return;

                    std::string propertyToEdit = line[3];
                    std::string propertyValNew = line[4];
                    set_property_keyframe(propertyToEdit, propertyValNew);
                }
                else if (line[1] == "-composition" || line[1] == "-c")
                {
                    std::string propertyToEdit = line[3];
                    std::string propertyValNew = line[4];
                    set_property_composition(propertyToEdit, propertyValNew);
                }
            }
        }
    }
    else if (line[0] == "hrir")
    {
        loadHRIR(line[1]);
    }
    else
    {
        std::cout << "Input Not Recognized!" << std::endl;
    }
};

void Interfacer::set_property_source(std::string propertyName, std::string propertyValue)
{
    /*int p = -1;
	for(int i = 0; i < END; i++){
		if(propertyName == SSPNames[i]){
			p = i;
			break;
		}
	}*/
    if (propertyName == "name")
    {
		printf("Set name to : %s\n", propertyValue.c_str());

        anime->getSource(selectedSource)->source->setName(propertyValue);
        selectedSource = propertyValue;
    }
    else if (propertyName == "start_time")
    {
        try
        {
            double value = (double)std::stod(propertyValue);
            anime->getSource(selectedSource)->timeStart_s = value;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: "
                      << "start_time"
                      << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
    } else if (propertyName == "visible"){
		//technically treats values other than true as false
		bool bval = (propertyValue == "T" || propertyValue == "t" || propertyValue == "true" || propertyValue == "TRUE" || propertyValue == "True");
		anime->getSource(selectedSource)->isVisible = bval;
		std::cout << bval << " " << propertyValue << " " <<anime->getSource(selectedSource)->isVisible<<std::endl;
	}

    else
    {
        std::cout << "ERROR: Not A source Property!" << std::endl;
    }
};

void Interfacer::set_property_keyframe(std::string propertyName, std::string propertyValue)
{
    int p = -1;
    for (int i = 0; i < END; i++)
    {
        if (propertyName == SSPNames[i])
        {
            p = i;
            break;
        }
    }
    double value = -1;
    bool bval = false;
    switch (p)
    {
	case TIME:
        //std::cout << "it worked" << std::endl;
        try
        {
            value = (double)std::stod(propertyValue);
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->time_s = value;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    case RADIUS:
        std::cout << "it worked" << std::endl;
        try
        {
            value = (double)std::stod(propertyValue);
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->properties->position->radius = value;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    case THETA:
        try
        {
            value = (double)std::stod(propertyValue);
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->properties->position->theta = value;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    case PHI:
        try
        {
            value = (double)std::stod(propertyValue);
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->properties->position->phi = value;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    case SCALE:
        try
        {
            value = (double)std::stod(propertyValue);
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->properties->scale = value;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    case LOOPING:
        try
        {
            //technically treats values other than true as false
            bval = (propertyValue == "T" || propertyValue == "t" || propertyValue == "true" || propertyValue == "TRUE" || propertyValue == "True");
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->properties->isLooping = bval;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    case VISIBLE:
        try
        {
            //technically treats values other than true as false
            bval = (propertyValue == "T" || propertyValue == "t" || propertyValue == "true" || propertyValue == "TRUE" || propertyValue == "True");
            anime->getSource(selectedSource)->keyFrameList.at(keyFrameSelected)->properties->isVisible = bval;
        }
        catch (const std::invalid_argument &e)
        {
            std::cout << "Error setting property: " << SSPNames[p] << " of Key Frame " << keyFrameSelected << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
        break;
    default:
        std::cout << "ERROR: Not A Key Frame Property!" << std::endl;
    }
}

void Interfacer::set_property_composition(std::string propertyName, std::string propertyValue)
{
    /*int p = -1;
	for(int i = 0; i < END; i++){
		if(propertyName == SSPNames[i]){
			p = i;
			break;
		}
	}*/
    if (propertyName == "name")
    {
        //anime->getSource(selectedSource)->source->setName(propertyValue);
        selectedComposition = propertyValue;
    }
    else if (propertyName == "start_time")
    {
        try
        {
            //double value = (double)std::stod(propertyValue);
            //anime->getSource(selectedSource)->timeStart_s = value;
        }
        catch (const std::invalid_argument &e)
        {
            //std::cout << "Error setting property: " << "start_time" << " of source \"" << selectedSource << "\": Property Value invalid!" << std::endl;
        }
    } else if (propertyName == "hrir")
    {
		std::cout << propertyValue << std::endl;
        loadHRIR(propertyValue);
    }
    else
    {
        std::cout << "ERROR: Not A composition Property!" << std::endl;
    }
}

int Interfacer::export_final(std::string filename)
{
    return 0;
}

//defaults to loading subject 40
void Interfacer::loadHRIR() {
    anime->reInitHRIR("../../data/CIPIC_hrtf_database/standard_hrir_database/subject_040/hrir_final.mat");
}

//attempts to load the selected subject, defaults to 40 if not found
void Interfacer::loadHRIR(std::string subject) {
    std::string filepath;
	int i = 0;
	for(i = 0; i < 45; i++){
		if(subject == subjects[i]){
			break;
		}
	}
    if(i != 45)
        filepath = "../../data/CIPIC_hrtf_database/standard_hrir_database/subject_"+subjects[i]+"/hrir_final.mat";
    else
        filepath = "../../data/CIPIC_hrtf_database/standard_hrir_database/subject_040/hrir_final.mat";

		printf( "%s\n", filepath.c_str());

    anime->reInitHRIR(filepath);
}
