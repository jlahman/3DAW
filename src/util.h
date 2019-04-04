#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include<iostream>

//TODO: add a function to convert to polar coords?
struct Point3D {
  public:
   Point3D(double x, double y, double z){
     this->x = x;
     this->y = y;
     this->z = z;
   };
   double x;
   double y;
   double z;
 };

int loadAudioData(std::string filepath, int sampleRate, int channels, double ** data, int *length);

void convolve( double* Signal, uint32_t SignalLen, double* Kernel, uint32_t KernelLen, double * output);

#endif
