#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H
#include<iostream>
#include<vector>
#include"SoundSource.h"
#include"HRIR_Data.h"
#include"KeyFrame.h"

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
    void addSource(std::string sourceName, Track * track);

    //this handles the convolution and addition, but probably shouldn't be a part of this class
    //animation player should just handle doing the interpolation of source properties between keyframes
    //    given a timestamp and frame length, maybe?
    void getBuffer(double ** buffer, int frameStart, int length);
    //std::vector<SoundSource*> getSources(double time_s);
    int addKeyFrame(std::string sourceName, double time_s, SoundSourceProperties * properties);
    int setStartTime(std::string sourceName, double time_s);



  private:
    //should AnimationPlayer just have a list of sound source properties, as thats the only thing being animated?
    //    a audioprocessor would querry properties at a timestamp and process the audio tracks accordingly,
    //        basically whats in getbuffer
    std::vector<MasterSource*> sourceList;

    HRIR_Data * hrir;
    //void processFrame(double ** buffer, int frameStart, int length);

};
#endif
