#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include<iostream>
#include"Track.h"
#include"util.h"

struct SoundSourceProperties {
  public:
   SoundSourceProperties(Polar3D * p, bool loop, bool visible);
   Polar3D * position;
   double scale;
   bool isLooping;
   bool isVisible;
};

class SoundSource {
  //member variables
  //TODO: make a struct for tract properties? need easy access to edit any properties
  //Just make track a data structure? no operations?
  public:
   SoundSource(Track * t);
   SoundSource(Track * t, std::string name);
   SoundSource(Track * t, std::string name, SoundSourceProperties * p);
   void setProperties(SoundSourceProperties * properties);
   SoundSourceProperties * getProperties();

   void setStartTime(double time);
   double getStartTime();

   double * getAudioData();
   int getLength();

   std::string getName();
  private:
   //Fields
   Track *track;
   std::string name;
   double startTime_s;
   SoundSourceProperties *properties;

   //Functions
   void initProperties();

};





#endif
