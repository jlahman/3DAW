#ifndef TRACT_H
#define TRACT_H
#include<iostream>

class Tract {
  //member variables
  public:
   Tract(std::string filepath);
   int getSampleRate();
   int getLength();
   double getAzimuth();
   void setAzimuth(double azimuth);
   double* * getData();


  private:
   std::string filepath;
   double * data;
   int sampleRate;
   int length;
   double azimuth, radius;

   int loadProperties();
};
#endif
