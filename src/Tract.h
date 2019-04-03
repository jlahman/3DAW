#ifndef TRACT_H
#define TRACT_H
#include<iostream>

class Tract {
  //member variables
  //TODO: make a struct for tract properties? need easy access to edit any properties
  //Just make track a data structure? no operations?
  public:
   Tract(std::string filepath, std::string name);
   int getSampleRate();
   int getLength();
   double getAzimuth();
   void setAzimuth(double azimuth);
   double* * getData();


  private:
   std::string filepath;
   std::string name;
   double * data;
   int sampleRate;
   int length;
   double azimuth, radius;
   bool isLooping;

   int loadProperties();
};
#endif
