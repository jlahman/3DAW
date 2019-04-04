#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H
#include<iostream>
#include<vector>
#include"SoundSource.h"
#include"HRIR_Data.h"

class AnimationPlayer {

  public:
    AnimationPlayer(std::string filepath);
    void addSource(std::string sourceName, Track * track);

    //this handles the convolution and addition, but probably shouldn't be a part of this class
    //animation player should just handle doing the interpolation of source properties between keyframes
    //    given a timestamp and frame length, maybe?
    void getBuffer(double ** buffer, int frameStart, int length);



  private:
    std::vector<SoundSource*> sourceList;
    HRIR_Data * hrir;
    //void processFrame(double ** buffer, int frameStart, int length);

};
#endif
