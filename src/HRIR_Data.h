#ifndef HRIR_DATA_H
#define HRIR_DATA_H
#include<iostream>

class HRIR_Data {
  //member variables
  public:
   HRIR_Data(std::string filepath, int azi, int ele);
   int azimuthIndex, elevationIndex;

   double hrir_l[25][50][200];
   double hrir_r[25][50][200];
   double itd[25][50];
   int theta;

  private:
   std::string filepath;

   void loadHRIRFromMAT();
   void loadITDFromMAT();
};
#endif
