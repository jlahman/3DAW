#include <stdio.h>
#include <stdlib.h>
#include"Track.h"
#include"util.h"

Track::Track(std::string filepath){
  this->filepath = filepath;
  this->name = filepath;
  loadProperties();
}

Track::Track(std::string filepath, std::string name){
  this->filepath = filepath;
  this->name = name;
  loadProperties();
}

int Track::loadProperties(){
  sampleRate = 44100;
  loadAudioData(filepath, sampleRate, 1, &data, &length);

}
