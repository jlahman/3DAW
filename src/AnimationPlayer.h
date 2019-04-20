#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H
#include<iostream>
#include<vector>
#include"SoundSource.h"
#include"HRIR_Data.h"
#include"KeyFrame.h"

//mastersource might be better made as just soundsource
//Need to plan out a new design with new knowledge
struct MasterSource {
  MasterSource(SoundSource * src, double start, double end){
    this->source = src;
    this->timeStart_s = start;
    this->timeFinal_s = end;
  };
  SoundSource * source;
  double timeStart_s;
  double timeFinal_s;
  std::vector<KeyFrame*> keyFrameList;
};

class AnimationPlayer {

  public:
    AnimationPlayer(std::string filepath);
    void reInitHRIR(std::string filepath)
    void addSource(std::string sourceName, Track * track);
	void removeSource(std::string sourceNam);
	bool hasSource(std::string sourceName);
	SoundSource* getSource(MasterSource * s, double time_s);
	MasterSource* getSource(std::string s);
	MasterSource* getSourceAt(int index);


    //this handles the convolution and addition, but probably shouldn't be a part of this class
    //animation player should just handle doing the interpolation of source properties between keyframes
    //    given a timestamp and frame length, maybe?
    void getBuffer(double ** buffer, double ** overflow, int frameStart, int length);
    std::vector<SoundSource*> getSources(double time_s);
	std::vector<SoundSource*> getSources();
    int addKeyFrame(std::string sourceName, double time_s, SoundSourceProperties * properties);
    int test_KeyFrames(std::string sourceName);

    int setStartTime(std::string sourceName, double time_s);



  private:
    //should AnimationPlayer just have a list of sound source properties, as thats the only thing being animated?
    //    a audioprocessor would querry properties at a timestamp and process the audio tracks accordingly,
    //        basically whats in getbuffer
    std::vector<MasterSource*> sourceList;
    //SoundSource* getSource(MasterSource * s, double time_s);
    SoundSourceProperties * interpolateProperties(MasterSource * s, double time_s, int index_lo, int index_hi);
    double * interpolateHRIR_linear( double index_a, double index_e, bool left, double * arr);



    HRIR_Data * hrir;
	double * convDataL;
	double * convDataR;
	double * mDataChunk;
	double * hrirLL, * hrirLR;

    //void processFrame(double ** buffer, int frameStart, int length);

};
#endif