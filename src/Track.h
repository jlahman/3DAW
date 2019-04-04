#ifndef TRACT_H
#define TRACT_H
#include<iostream>

struct Track {
  //member variables
  //TODO: make a struct for tract properties? need easy access to edit any properties
  //Just make track a data structure? no operations?
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
