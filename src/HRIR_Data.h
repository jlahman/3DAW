#ifndef HRIR_DATA_H
#define HRIR_DATA_H
#include<iostream>

class HRIR_Data {
  //member variables
  public:
   HRIR_Data(std::string filepath);
   double * getIndices(double azimuth, double elevation);
   double azimuths[27];
   double elevations[50];
   double hrir_l[27][50][200];
   double hrir_r[27][50][200];
   double itd[25][50];
   int theta;

  private:
   std::string filepath;
   double findClosestWeightedIndex(double * arr, int legnth, double value);
   void initAngles();
   void loadHRIRFromMAT();
   void genPIHRIR();
   void loadITDFromMAT();
};
#endif
