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
	~Interfacer();

 	enum SSPEnums					{POSITION,	 	RADIUS, 	THETA ,		PHI,	SCALE,		LOOPING , 	VISIBLE, END  };
	const std::string SSPNames[8] = {"position", 	"radius",	"theta",	"phi",	"scale",	"looping",	"visible", "invalid" };

	void handle_input(std::string line);
	int myMain();

private:
	AudioPlayer ap = AudioPlayer();
	AnimationPlayer * anime = new AnimationPlayer("../../data/CIPIC_hrtf_database/standard_hrir_database/subject_040/hrir_final.mat");
	std::vector<Track*> trackList;

	const int frameStop = 44100 * 180 ;
	const int framesPerAnimationStep = 1024;

	int frameCount = 0;
	bool animePlay = false;
	bool done = false;
	bool pause = true;

	std::string selectedSource = "";
	std::string selectedComposition = "";

	int keyFrameSelected = 0;
	double ** audioOut = NULL;
	double ** overflow = NULL;

	std::vector<std::string> split(const std::string& s, char delimiter);
	void foo();

	int export_final(std::string filepath);

	void set_property_keyframe(std::string propertyName, std::string propertyValue);
	void set_property_source(std::string propertyName, std::string propertyValue);
	void set_property_composition(std::string propertyName, std::string propertyValue);

};
#endif
