#include<iostream>
#include<cmath>
#include"AnimationPlayer.h"
#include"util.h"

AnimationPlayer::AnimationPlayer(std::string filepath){
  hrir = new HRIR_Data(filepath);
}

void AnimationPlayer::addSource(std::string sourceName, Track * track){
  sourceList.push_back(new MasterSource(new SoundSource(track, sourceName), 0.0, 0.0));
}

int AnimationPlayer::addKeyFrame(std::string sourceName, double time_s, SoundSourceProperties * ssp){
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    if((*source)->source->getName().compare(sourceName) == 0){
      (*source)->keyFrameList.push_back(new KeyFrame(ssp, time_s));
      return 0;
    }
  }
  return -1;
}

int AnimationPlayer::test_KeyFrames(std::string sourceName){
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    if((*source)->source->getName().compare(sourceName) == 0){
      SoundSourceProperties* p = new SoundSourceProperties( new Polar3D( (*source)->source->getProperties()->position->radius,
                                                                        (*source)->source->getProperties()->position->theta -100,
                                                                        (*source)->source->getProperties()->position->phi),
                                                          true,
                                                          true);
      double time_s = 0.0;

      (*source)->keyFrameList.push_back(new KeyFrame(p, time_s));
      for(int i = 1; i < 60; i++){
        p = new SoundSourceProperties(new Polar3D( p->position->radius, p->position->theta, p->position->phi),
                                      true,
                                      true);
        double delta = 5;
        p->position->theta = p->position->theta + 180.0 + delta;
        p->position->theta = fmod(p->position->theta, 360.0);
        p->position->theta = p->position->theta - 180;
        p->scale = 0.5;
        //printf("%d\n", (int) p->position->theta);
        time_s = time_s + .25;
        (*source)->keyFrameList.push_back(new KeyFrame(p, time_s));
      }

      return 0;

    }
  }
  return -1;
}


int AnimationPlayer::setStartTime(std::string sourceName, double time_s){
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    if((*source)->source->getName().compare(sourceName) == 0){
      (*source)->timeStart_s = time_s;
      return 0;
    }
  }
  return -1;
}

std::vector<SoundSource*> AnimationPlayer::getSources(double time_s){
  std::vector<SoundSource*> returnList;
  for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
    SoundSource * temp = getSource((*source), time_s);
    if(temp){
      returnList.push_back(temp);
    }
  }
  return returnList;
}

SoundSource* AnimationPlayer::getSource(MasterSource  *s, double time_s){
  SoundSource* instSource = NULL;
  if(s->timeStart_s > time_s){
    return NULL;
  } else{
    time_s -= s->timeStart_s;
  }
  if(s->keyFrameList.empty()){
    instSource = s->source;
  } else if(s->keyFrameList.size() == 1) {
    instSource = s->source;
    instSource->setProperties(s->keyFrameList.at(0)->properties);
  } else {
    instSource = s->source;

    //findClosestIndex()
    //this search is O(n) aka kinda awful
    int index_lo = -1;
    int index_hi = -1;

    double time_lo = INFINITY;//(time_s - s->keyFrameList.at(0)->time_s);
    double time_hi = INFINITY;// abs(s->keyFrameList.at(0)->time_s - time_s);

    KeyFrame * kf;
    for( int i = 0; i < s->keyFrameList.size(); i++){
      kf = s->keyFrameList.at(i);
      if(abs(kf->time_s - time_s) < time_hi && (kf->time_s - time_s) >= 0){//int e = 0;std::cin >> e;
        index_hi = i;
        time_hi = kf->time_s - time_s;
      }
      if(abs(time_s - kf->time_s) < time_lo && (time_s - kf->time_s) >= 0){
        index_lo = i;//int e = 0;std::cin >> e;
        time_lo = time_s - kf->time_s;
      }
    }
    //end findClosestIndex

    //interp between unless equal
    if(index_lo == index_hi || (index_hi == -1 && index_lo != -1))
      instSource->setProperties(s->keyFrameList.at(index_lo)->properties);
    else
      instSource->setProperties(interpolateProperties(s, time_s, index_lo, index_hi));

  //  printf("Index high at %E(seconds):\t%d\t%d\n", time_s, index_lo, index_hi);
  }

  return instSource;
}

SoundSourceProperties * AnimationPlayer::interpolateProperties(MasterSource * s, double time_s, int index_lo, int index_hi){
  SoundSourceProperties * ssp = NULL;
  SoundSourceProperties * lo =  s->keyFrameList.at(index_lo)->properties;
  SoundSourceProperties * hi =  s->keyFrameList.at(index_hi)->properties;

  double time_lo = s->keyFrameList.at(index_lo)->time_s;
  double time_hi = s->keyFrameList.at(index_hi)->time_s;

  Polar3D * position = new Polar3D(1.0, 0.0, 0.0);
  position->radius = lerp(lo->position->radius, hi->position->radius, time_s, time_lo, time_hi);
  position->theta = lerp(lo->position->theta, hi->position->theta, time_s, time_lo, time_hi);
  position->phi = lerp(lo->position->phi, hi->position->phi, time_s, time_lo, time_hi);
  //printf("Radius: %E\t theta: %E\tPHI: %E\n", lo->position->radius, lo->position->theta, lo->position->phi);

  double scale = lerp(lo->scale, hi->scale, time_s, time_lo, time_hi);

  ssp = new SoundSourceProperties(position, lo->isLooping, lo->isVisible);
  ssp->scale = scale;
  return ssp;
}

double * AnimationPlayer::interpolateHRIR_linear(double index_a, int index_e, bool left){
  double * hrirLerped = new double[200];
  int lower = (int)index_a;
  int upper = (int)index_a+1;
  if(upper > 26)
    upper = 26;
  for(int i = 0; i < 200; i++){
    if(left)
      hrirLerped[i] = lerp(hrir->hrir_l[lower][(int)index_e][i], hrir->hrir_l[upper][(int)index_e][i], index_a, lower, upper);
    else
      hrirLerped[i] = lerp(hrir->hrir_r[lower][(int)index_e][i], hrir->hrir_r[upper][(int)index_e][i], index_a,lower, upper);
  }
  return hrirLerped;
}

void AnimationPlayer::getBuffer(double ** buffer, double ** overflow, int frameStart, const int length){
  const int hrirLength = 200;
  const int convDataSize = length + hrirLength - 1;
  const int sourceChunkSize = length;
  //frameStart = frameStart - 197;
  	double * audioData;

	//shouldn't be doing memory allocation or deallocation in paCallBackMethod
	//printf("BeforeDelete\n" );
//	if(mDataChunk != NULL) delete[] mDataChunk;
	//printf("AfterDelete\n" );
 	mDataChunk = new double[sourceChunkSize];
	//printf("afterNew\n");
	//if(convDataL != NULL)
	//delete[] convDataL;
	//if(convDataR != NULL)
	//delete[] convDataR;
	convDataL = new double[convDataSize];
	convDataR = new double[convDataSize];

  double frameTime = frameStart/44100.0;

  std::vector<SoundSource*> ssl = getSources(frameTime);


  for(int i =0; i< length; i++){
    buffer[0][i] = 0;
    buffer[1][i] = 0;
  }

  for(int i =0; i< 200; i++){
    overflow[0][i] = 0;
    overflow[1][i] = 0;
  }

  if(ssl.empty()){
    return;
  }



  for(std::vector<SoundSource*>::iterator source = ssl.begin(); source != ssl.end(); source++){
    audioData = (*source)->getAudioData();
    //printf("Audio Length%d\t%E\n", (*source)->getLength(),(*source)->getLength()/44100.0 );
    double start = 0.0;//(*source)->timeStart_s;

    for(int i = 0; i < sourceChunkSize; i++){
      if(start > frameTime){
        mDataChunk[i] = 0;
      }
      else{
        if((*source)->getProperties()->isLooping){
          double scale = (*source)->getProperties()->scale;
          mDataChunk[i] = scale*audioData[(i+frameStart)%(*source)->getLength()];
        //  if((i+frameStart)%(*source)->getLength() < 100 || (i+frameStart)%(*source)->getLength() > (*source)->getLength() -100)
        //  printf("AudioDataChunk[%d]: %E\t|\t%E :[%d]\n", i, mDataChunk[i],  scale*audioData[(i+frameStart)%(*source)->getLength()], (i+frameStart)%(*source)->getLength());
        }
        else {
          if(i + frameStart >= (*source)->getLength()){
            mDataChunk[i] = 0;
          } else{
            double scale = (*source)->getProperties()->scale;
            mDataChunk[i] = scale* audioData[i+frameStart];
          }
        }
      }
      frameTime += 1.0/44100.0;
    }

    Polar3D *p = (*source)->getProperties()->position;

    double aziIndex = hrir->getIndices(p->theta, p->phi)[0];

    int eleIndex = (int)hrir->getIndices(p->theta, p->phi)[1];
    double * hrirLL, *hrirLR;
    hrirLL = interpolateHRIR_linear(aziIndex, eleIndex, true);
    hrirLR = interpolateHRIR_linear(aziIndex, eleIndex, false);
    //printf("%d\n", (int)p->theta);
    convolve(mDataChunk, sourceChunkSize, hrirLL, 200, convDataL);
    convolve(mDataChunk, sourceChunkSize, hrirLR, 200, convDataR);

    for(int i =0; i< length; i++){
      buffer[0][i] += convDataL[i];
      buffer[1][i] += convDataR[i];
      if(abs(buffer[0][i]) > 1.0){
        buffer[0][i] = buffer[0][i]/abs(buffer[0][i]);
      }
      if(abs(buffer[1][i]) > 1.0){
        buffer[1][i] = buffer[1][i]/abs(buffer[1][i]);
      }
      //if(i < 100 || i > length -100)
      //printf("AudioDataChunk[%d]: %E\t|\t :[]\n", i, buffer[0][i]);
      if(abs(buffer[0][i] - buffer[0][i-1]) > 0.2)
      printf("%E\t%d\t%d\t%d\n", buffer[0][i], frameStart, i, (*source)->getLength());

    }
    for(int i =length; i< length + 199; i++){
      overflow[0][i-length] += convDataL[i];
      overflow[1][i-length] += convDataR[i];
    /*  if(abs(overflow[0][i-length]) > 1.0){
        overflow[0][i-length] = overflow[0][i-length]/abs(overflow[1][i-length]);
      }
      if(abs(overflow[1][i-length]) > 1.0){
        overflow[1][i-length] = overflow[1][i-length]/abs(overflow[1][i-length]);
      }*/
      //printf("%E\t%d\t%d\t%d\n", buffer[0][i], frameStart, i, (*source)->getLength());

    }

  }
  if(convDataL != NULL and convDataR != NULL){
	//  delete[] convDataL;
	//  convDataL = NULL;
	//  delete[] convDataR;
	// convDataR = NULL;
	 //delete[] hrirL; hrirL = NULL;
    // delete[] hrirR; hrirR = NULL;

  }

  //delete[] convDataR;
  //delete[] mDataChunk;
  //mDataChunk = NULL;

/*
  for(int i =0; i< 200; i++){
    buffer[0][i] = lerp(0, buffer[0][199], i, 0, 200-1);
    buffer[1][i] = lerp(0, buffer[1][199], i, 0, 200-1);



  }
  for(int i =length -200; i< length; i++){
    buffer[0][i] = lerp(buffer[0][length -200], 0, i, length -200, length-1);
    buffer[1][i] = lerp(buffer[1][length -200], 0, i, length -200, length-1);


  }*/
}
