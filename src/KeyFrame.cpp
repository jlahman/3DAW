#include"KeyFrame.h"

KeyFrame::KeyFrame(SoundSourceProperties * ssp, double time_s){
  this->properties = ssp;
  this->time_s = time_s;
}
