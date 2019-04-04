#include "SoundSource.h"

SoundSource::SoundSource(Track * t){
  this->track = t;
  this->name = track->name;
  initProperties();
}

SoundSource::SoundSource(Track * t, std::string name){
  this->track = t;
  this->name = name;
  initProperties();
}

SoundSource::SoundSource(Track * t, std::string name, SoundSourceProperties * properties){
  this->track = t;
  this->name = name;
  this->properties = properties;
}

double * SoundSource::getAudioData(){
  return track->data;
}

int SoundSource::getLength(){
  return track->length;
}

void SoundSource::setProperties(SoundSourceProperties * properties){
  this->properties = properties;
}

SoundSourceProperties * SoundSource::getProperties(){
  return properties;
}

void SoundSource::setStartTime(double time_s){
  this->startTime_s = time_s;
}

double SoundSource::getStartTime(){
  return startTime_s;
}

void SoundSource::initProperties(){
  properties = new SoundSourceProperties(new Point3D(0.70710678118, 0.70710678118, 0.0), true, true);
}


SoundSourceProperties::SoundSourceProperties(Point3D * p, bool looping, bool visible){
  this->position = p;
  this->isLooping = looping;
  this->isVisible = visible;
}
