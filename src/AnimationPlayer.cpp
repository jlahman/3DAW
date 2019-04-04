#include<iostream>
#include"AnimationPlayer.h"
#include<cmath>
#include"util.h"

AnimationPlayer::AnimationPlayer(std::string filepath){
  hrir = new HRIR_Data(filepath);

}

void AnimationPlayer::addSource(std::string sourceName, Track * track){
  sourceList.push_back(new SoundSource(track, sourceName));
}

void AnimationPlayer::getBuffer(double ** buffer, int frameStart, int length){
  double * mDataChunk = new double[length-199];
  double * audioData;

  int convDataSize = length;
  double * cData = new double[convDataSize];
  double * cDataR = new double[convDataSize];

  for(int i =0; i< length; i++){
    buffer[0][i] = 0;
    buffer[1][i] = 0;
  }

  for(std::vector<SoundSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    audioData = (*source)->getAudioData();
    for(int i = 0; i < length - 199; i++){

      if((*source)->getProperties()->isLooping){
        mDataChunk[i] = audioData[(i+frameStart)%(*source)->getLength()];
      }
      else {
        if(i + frameStart >= (*source)->getLength()){
          mDataChunk[i] = 0;
        } else{
          mDataChunk[i] = audioData[i+frameStart];
        }
      }
    }
    Point3D *p = (*source)->getProperties()->position;
    double azimuth = atan2(p->x, p->y);
    azimuth = azimuth * 180 / 3.14159265;
    //printf("%E\n", azimuth);

    int aziIndex = hrir->getIndices(azimuth, 0)[0];
    int eleIndex = hrir->getIndices(azimuth, 0)[1];
    convolve(mDataChunk, length - 199, hrir->hrir_l[aziIndex][eleIndex], 200, cData);
    convolve(mDataChunk, length - 199, hrir->hrir_r[aziIndex][eleIndex], 200, cDataR);

    for(int i =0; i< length; i++){
      buffer[0][i] += cData[i];
      buffer[1][i] += cDataR[i];
      //printf("%E\t%d\t%d\t%d\n", buffer[0][i], frameStart, i, (*source)->getLength());

    }
  }
}
