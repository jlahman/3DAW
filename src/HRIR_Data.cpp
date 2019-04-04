#include<matio.h>
#include"HRIR_Data.h"



HRIR_Data::HRIR_Data(std::string filepath){
  this->filepath = filepath;
  initAngles();
  loadITDFromMAT();
  loadHRIRFromMAT();
}

void HRIR_Data::initAngles(){
  azimuths[0] =  -80.0;
  azimuths[24] = 80.0;
  azimuths[1] = -65.0;
  azimuths[23] = 65.0;
  azimuths[2] =  -55.0;
  azimuths[22] = 55.0;
  for(int i = 3; i < 22; i++){
    azimuths[i] = -45 + 5*(i-3);
  }

  for(int i = 0; i < 50; i++){
    elevations[i] = -45 + 5.625*i;
  }
}


void HRIR_Data::loadHRIRFromMAT(){
	const char *filename = filepath.c_str();
	mat_t *matfp = NULL;
	matvar_t *mvarLeft = NULL;
	matvar_t *mvarRight = NULL;


	matfp = Mat_Open(filename, MAT_ACC_RDONLY);

	if(matfp){
		mvarLeft = Mat_VarRead(matfp, (char*)"hrir_l");
		mvarRight = Mat_VarRead(matfp, (char*)"hrir_r");

		const double *lData = (const double*)(mvarLeft->data);
		const double *rData = (const double*)(mvarRight->data);

		if(mvarLeft && mvarRight){
      for(int x = 0; x < 25; x++){
        for(int y = 0; y < 50; y++){
    			int skip = 25*50;
    			int c = 25*y + x;
    			for(int i = 0; i < 200; i++){
    				hrir_l[x][y][i] = lData[c+ skip*i];
    				hrir_r[x][y][i] = rData[c+ skip*i];

    			}
        }
      }
		}
		else{
			fprintf(stdout, "ERROR: mat variable read error");
		}
	}
	else {
		fprintf(stdout, "ERROR: Matfile load error");
	}
}

void HRIR_Data::loadITDFromMAT(){
	const char *filename = filepath.c_str();//"../data/CIPIC_hrtf_database/standard_hrir_database/subject_033/hrir_final.mat";
	mat_t *matfp = NULL;
	matvar_t *mvarITD = NULL;

	matfp = Mat_Open(filename, MAT_ACC_RDONLY);
	if(matfp){
		mvarITD = Mat_VarRead(matfp, (char*)"ITD");

		const double *itdData = (const double*)(mvarITD->data);

		if(mvarITD){
      for(int x = 0; x < 25; x++){
        for(int y = 0; y < 50; y++){
          int c = 25*y + x;
          itd[x][y] = itdData[c];
        }
      }
		}
		else{
			fprintf(stdout, "ERROR: mat variable read error");
		}
	}
	else {
		fprintf(stdout, "ERROR: Matfile load error");
	}
}

//This only returns the proper horizontal plane index, elevation is wonky
int* HRIR_Data::getIndices(double azi, double ele){
  int a, e;
  int *ret =  new int[2];

  //TODO: Figure out how to convert from interaural-polar coordinate system
  //      to vertical-polar coordinates.
  //conversely, we could use xyz as inputs from track data, instead of polar coords
  e = 8; // horizontal plane, front
  if(azi > 90){
    azi = 90 - (azi - 90);
    ele = ele + 180;
    e = 40; //horizontal plane, back
  } else if(azi < -90){
    azi = -90 - (azi + 90);
    ele = ele + 180;
    e = 40;// horizontal plane, back
  }
  a = (int)findClosestWeightedIndex(azimuths, 25, azi);
  ret[0] = a;
  ret[1] = e;
  return ret;
}

//move to util file? yes, then we can call this outside of hrirdata and use it to find the indices for interpolation
double HRIR_Data::findClosestWeightedIndex(double *arr, int length, double value){
  int index_o = 0;
  int index_f = length-1;
  bool done = false;
  while(index_o < index_f){
    int half = (index_f + index_o)/2;
    //base cases
    if(arr[half] == value){
      return (double)half;
    }
    if(value <= arr[index_o]){
      return (double)index_o;
    }
    if(value >= arr[index_f]){
      return (double)index_f;
    }
    //binary search cases
    if(value > arr[half]){
      if(half < index_f && value < arr[half+1] ){
        return (double)(((value - arr[half])/(arr[half+1] - arr[half] )) + half);
      }
      index_o = half;
    }
    else if (value < arr[half]){
      if(half > index_o && value > arr[half-1] ){
        return (double) (half - ((value - arr[half-1])/(arr[half] - arr[half-1] )));
      }
      index_f = half;
    }
  }

}
