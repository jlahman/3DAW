#ifndef INTERFACER_H
#define INTERFACER_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <sstream>
#include <fstream>

#include"Track.h"
#include"util.h"
#include"AudioPlayer.h"

#include"AnimationPlayer.h"


class Interfacer{
public:
	Interfacer();
	std::vector<Track*> trackList;
	AudioPlayer ap = AudioPlayer();
	AnimationPlayer * anime = new AnimationPlayer("../data/CIPIC_hrtf_database/standard_hrir_database/subject_059/hrir_final.mat");
	const int framesPerAnimationStep = 1024;
	int frameCount = 0;
	bool animePlay = false;
	bool done = false;
	bool pause = true;
	std::string selectedSource = "";
	int keyFrameSelected = 0;

	const int frameStop = 44100 * 180 ;

	double ** audioOut = NULL;
	double ** overflow = NULL;
	std::istream * inStream = &std::cin;

	enum SSPEnums	{POSITION,	 	RADIUS, 	THETA ,		PHI,	SCALE,		LOOPING , 	VISIBLE, END  };
	std::string SSPNames[8] = 	{"position", 	"radius",	"theta",	"phi",	"scale",	"looping",	"visible", "invalid" };

	void set_property_keyframe(std::string propertyName, std::string propertyValue);
	void set_property_source(std::string propertyName, std::string propertyValue);
	void foo();
	//void handle_input( std::istream *is);
	void handle_input( std::string line);

	int myMain();
	std::vector<std::string> split(const std::string& s, char delimiter);
	void setInStream(std::istream *inStream);

};
#endif
