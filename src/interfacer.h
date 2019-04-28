#ifndef INTERFACER_H
#define INTERFACER_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <sstream>
#include <fstream>

#include "Track.h"
#include "util.h"
#include "AudioPlayer.h"

#include "AnimationPlayer.h"

class Interfacer
{
  public:
    Interfacer();
    ~Interfacer();

    enum SSPEnums
    {
        POSITION,
        RADIUS,
        THETA,
        PHI,
        SCALE,
        LOOPING,
        VISIBLE,
        END
    };
    const std::string SSPNames[8] = {"position", "radius", "theta", "phi", "scale", "looping", "visible", "invalid"};

    void handle_input(std::string line);
	bool hasTrack(std::string name);
	int getTrackIndex(std::string name);
	int getTrackSize();
	bool pause = true;


    int myMain();

  private:
    AudioPlayer ap = AudioPlayer();
    AnimationPlayer *anime;
    std::vector<Track *> trackList;

    const int frameStop = 44100 * 180;
    const int framesPerAnimationStep = 1024;

    int frameCount = 0;
    bool animePlay = false;
    bool done = false;

    std::string selectedSource = "";
    std::string selectedComposition = "";

    int keyFrameSelected = 0;
    double **audioOut = NULL;
    double **overflow = NULL;

    std::vector<std::string> split(const std::string &s, char delimiter);
    void foo();

    int export_final(std::string filepath);

    void set_property_keyframe(std::string propertyName, std::string propertyValue);
    void set_property_source(std::string propertyName, std::string propertyValue);
    void set_property_composition(std::string propertyName, std::string propertyValue);

    void loadHRIR();
    void loadHRIR(std::string _subject);

    //45 entries
    std::string subjects[45] = {
        "003",
        "008",
        "009",
        "010",
        "011",
        "012",
        "015",
        "017",
        "018",
        "019",
        "020",
        "021",
        "027",
        "028",
        "033",
        "040",
        "044",
        "048",
        "050",
        "051",
        "058",
        "059",
        "060",
        "061",
        "065",
        "119",
        "124",
        "126",
        "127",
        "131",
        "133",
        "134",
        "135",
        "137",
        "147",
        "148",
        "152",
        "153",
        "154",
        "155",
        "156",
        "158",
        "162",
        "163",
        "165"};
};
#endif
