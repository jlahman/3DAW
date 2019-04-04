#ifndef TRACT_H
#define TRACT_H
#include<iostream>

struct Track {
  public:
   Track(std::string filepath);
   Track(std::string filepath, std::string name);

   std::string filepath;
   std::string name;
   double * data;
   int sampleRate;
   int length;
   int loadProperties();

};
#endif
