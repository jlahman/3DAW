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

 struct Polar3D{
 public:
  Polar3D(double radius, double theta, double phi){
    this->radius = radius;
    this->theta = theta;
    this->phi = phi;
  };
  double radius;
  double theta;
  double phi;
 };

int loadAudioData(std::string filepath, int sampleRate, int channels, double ** data, int *length);

void convolve( double* Signal, uint32_t SignalLen, double* Kernel, uint32_t KernelLen, double * output);

double lerp(double y0, double y1, double x, double x0, double x1);


#endif
