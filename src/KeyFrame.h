#ifndef KEYFRAME_H
#define KEYFRAME_H
#include<iostream>
#include"SoundSource.h"

class KeyFrame {
  public:
   KeyFrame(SoundSourceProperties * ssp, double time_s);

   double time_s;
   SoundSourceProperties * properties;
};
#endif
