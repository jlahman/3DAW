#ifndef TRACT_H
#define TRACT_H
#include<iostream>

class Tract {
  //member variables
  //TODO: make a struct for tract properties? need easy access to edit any properties
  //Just make track a data structure? no operations?
  public:
   Tract(std::string filepath, std::string name, double azimuth);
   int getSampleRate();
   int getLength();
   double getAzimuth();
   void setAzimuth(double azimuth);
   double* * getData();
   bool isLooping();


  private:
   std::string filepath;
   std::string name;
   double * data;
   int sampleRate;
   int length;
   double azimuth, radius;
   bool looping;

   int loadProperties();
};
#endif
