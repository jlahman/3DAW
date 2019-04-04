#include<iostream>
#include<cmath>
#include"AnimationPlayer.h"
#include"util.h"

AnimationPlayer::AnimationPlayer(std::string filepath){
  hrir = new HRIR_Data(filepath);
}

void AnimationPlayer::addSource(std::string sourceName, Track * track){
  sourceList.push_back(new MasterSource(new SoundSource(track, sourceName), 0.0, 0.0));
}

int AnimationPlayer::addKeyFrame(std::string sourceName, double time_s, SoundSourceProperties * ssp){
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    if((*source)->source->getName().compare(sourceName) == 0){
      (*source)->keyFrameList.push_back(new KeyFrame(ssp, time_s));
      return 0;
    }
  }
  return -1;
}

int AnimationPlayer::setStartTime(std::string sourceName, double time_s){
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    if((*source)->source->getName().compare(sourceName) == 0){
      (*source)->timeStart_s = time_s;
      return 0;
    }
  }
  return -1;
}
/*
std::vector<SoundSource*> AnimationPlayer::getSources(double time_s){
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){

  }
  return NULL;
}*/


void AnimationPlayer::getBuffer(double ** buffer, int frameStart, int length){
  int convDataSize = length;
  int sourceChunkSize = length - 200 + 1;

  double * mDataChunk = new double[sourceChunkSize];
  double * audioData;

  double * convDataL = new double[convDataSize];
  double * convDataR = new double[convDataSize];

  for(int i =0; i< length; i++){
    buffer[0][i] = 0;
    buffer[1][i] = 0;
  }

  double frameTime = frameStart/44100.0;

  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    audioData = (*source)->source->getAudioData();
    double start = (*source)->timeStart_s;

    for(int i = 0; i < sourceChunkSize; i++){
      if(start > frameTime){
        mDataChunk[i] = 0;
      }
      else{
        if((*source)->source->getProperties()->isLooping){
          mDataChunk[i] = audioData[(i+frameStart)%(*source)->source->getLength()];
        }
        else {
          if(i + frameStart >= (*source)->source->getLength()){
            mDataChunk[i] = 0;
          } else{
            mDataChunk[i] = audioData[i+frameStart];
          }
        }
      }
      frameTime += 1.0/44100.0;
    }

    Polar3D *p = (*source)->source->getProperties()->position;
    /*
    double azimuth = atan2(p->x, p->y);
    azimuth = azimuth + 3.14159265 + 3.14159265/20.0;
    azimuth = fmod(azimuth, 2*3.14159265);
    azimuth = azimuth - 3.14159265;
    p->x = sin(azimuth);
    p->y = cos(azimuth);
    azimuth = azimuth * 180 / 3.14159265;*/

    double delta = 5.0;
    p->theta = p->theta + 180.0 + delta;
    p->theta = fmod(p->theta, 360.0);
    p->theta = p->theta - 180;

    int aziIndex = hrir->getIndices(p->theta, p->phi)[0];
    int eleIndex = hrir->getIndices(p->theta, p->phi)[1];
    //printf("%d\n", eleIndex);
    convolve(mDataChunk, sourceChunkSize, hrir->hrir_l[aziIndex][eleIndex], 200, convDataL);
    convolve(mDataChunk, sourceChunkSize, hrir->hrir_r[aziIndex][eleIndex], 200, convDataR);

    for(int i =0; i< length; i++){
      buffer[0][i] += convDataL[i];
      buffer[1][i] += convDataR[i];
      //printf("%E\t%d\t%d\t%d\n", buffer[0][i], frameStart, i, (*source)->getLength());

    }

  }
}
