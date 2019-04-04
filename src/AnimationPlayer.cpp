#include<iostream>
#include<cmath>
#include"AnimationPlayer.h"
#include"util.h"

AnimationPlayer::AnimationPlayer(std::string filepath){
  hrir = new HRIR_Data(filepath);
}

void AnimationPlayer::addSource(std::string sourceName, Track * track){
  sourceList.push_back(new SoundSource(track, sourceName));
}

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

  for(std::vector<SoundSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    audioData = (*source)->getAudioData();
    for(int i = 0; i < sourceChunkSize; i++){

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
    //azimuth = azimuth * 180 / 3.14159265;
    azimuth = azimuth + 3.14159265 + 3.14159265/20.0;
    azimuth = fmod(azimuth, 2*3.14159265);
    azimuth = azimuth - 3.14159265;
    p->x = sin(azimuth);
    p->y = cos(azimuth);
    azimuth = azimuth * 180 / 3.14159265;

    int aziIndex = hrir->getIndices(azimuth, 0)[0];
    int eleIndex = hrir->getIndices(azimuth, 0)[1];
    convolve(mDataChunk, sourceChunkSize, hrir->hrir_l[aziIndex][eleIndex], 200, convDataL);
    convolve(mDataChunk, sourceChunkSize, hrir->hrir_r[aziIndex][eleIndex], 200, convDataR);

    for(int i =0; i< length; i++){
      buffer[0][i] += convDataL[i];
      buffer[1][i] += convDataR[i];
      //printf("%E\t%d\t%d\t%d\n", buffer[0][i], frameStart, i, (*source)->getLength());

    }

  }
}
