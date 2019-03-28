#include"HRIR_Data.h"
#include<matio.h>


HRIR_Data::HRIR_Data(std::string filepath, int azi, int ele){
  filepath = filepath;
  azimuthIndex = azi;
  elevationIndex = ele;

  loadITDFromMAT();
  loadHRIRFromMAT();
}

void HRIR_Data::loadHRIRFromMAT(){
	const char *filename = "CIPIC_hrtf_database/standard_hrir_database/subject_033/hrir_final.mat";
	mat_t *matfp = NULL;
	matvar_t *mvarLeft = NULL;
	matvar_t *mvarRight = NULL;


	matfp = Mat_Open(filename, MAT_ACC_RDONLY);

	if(matfp){
		mvarLeft = Mat_VarRead(matfp, (char*)"hrir_l");
		mvarRight = Mat_VarRead(matfp, (char*)"hrir_r");

		unsigned lSize = mvarLeft->nbytes/mvarLeft->data_size/200;
		unsigned rSize = mvarRight->nbytes/mvarRight->data_size/200;
		const double *lData = (const double*)(mvarLeft->data);
		const double *rData = (const double*)(mvarRight->data);

		if(mvarLeft && mvarRight){
      for(int x = 0; x < 25; x++){
        for(int y = 0; y < 50; y++){
    			// y = elevationIndex;
    			//x = azimuthIndex;
    			int skip = 25*50;
    			int c = 25*y + x;
    			for(int i = 0; i < 200; i++){
    				hrir_l[x][y][i] = lData[c+ skip*i];
    				hrir_r[x][y][i] = rData[c+ skip*i];
    				//printf("%E\t", hrir[0][i]); 				printf("%d\t%d\n", (c + skip*i), i);

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
	const char *filename = "CIPIC_hrtf_database/standard_hrir_database/subject_033/hrir_final.mat";
	mat_t *matfp = NULL;
	matvar_t *mvarITD = NULL;

	matfp = Mat_Open(filename, MAT_ACC_RDONLY);
	if(matfp){
		mvarITD = Mat_VarRead(matfp, (char*)"ITD");

		unsigned itdSize = mvarITD->nbytes/mvarITD->data_size;
		const double *itdData = (const double*)(mvarITD->data);

		if(mvarITD){
			int y = elevationIndex;
			//int x = elevationIndex;
			int x = azimuthIndex;
			int c = 25*y + x;
      for(int x = 0; x < 25; x++){
        for(int y = 0; y < 50; y++){
          // y = elevationIndex;
          //x = azimuthIndex;
          int skip = 25*50;
          int c = 25*y + x;
          itd[x][y] = itdData[c];
          //printf("%E\t", hrir[0][i]); 				printf("%d\t%d\n", (c + skip*i), i);
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
