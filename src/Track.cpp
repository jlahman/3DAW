#include"Track.h"
#include"util.h"
#include <stdio.h>
#include <stdlib.h>


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

//TODO: Move this to a util file probably
int Track::loadProperties(){
  sampleRate = 44100;
  loadAudioData(filepath, sampleRate, 1, &data, &length);

}
