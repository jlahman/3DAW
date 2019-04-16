#include<iostream>
#include<cmath>
#include"AnimationPlayer.h"
#include"util.h"

AnimationPlayer::AnimationPlayer(std::string filepath){
	hrir = new HRIR_Data(filepath);
	hrirLL = new double[200];
	hrirLR = new double[200];
}

bool AnimationPlayer::hasSource(std::string sourceName){
	for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
		if((*source)->source->getName().compare(sourceName) == 0){
			return true;
		}
	}
	return false;
}

MasterSource * AnimationPlayer::getSource(std::string sourceName){
	for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
		if((*source)->source->getName().compare(sourceName) == 0){
			return (*source);
		}
	}
	return NULL;
}

void AnimationPlayer::removeSource(std::string sourceName){
	for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
		if((*source)->source->getName().compare(sourceName) == 0){
			delete (*source);
			sourceList.erase(source);
			return;
		}
	}
	return;
}

MasterSource * AnimationPlayer::getSourceAt(int index){
	if(index >= sourceList.size() || index < 0)
		return NULL;

	return sourceList.at(index);
}

void AnimationPlayer::addSource(std::string sourceName, Track * track){
	  sourceList.push_back(new MasterSource(new SoundSource(track, sourceName), 0.0, 0.0));

	  SoundSourceProperties* p = new SoundSourceProperties( new Polar3D( sourceList.at(sourceList.size()-1)->source->getProperties()->position->radius,
																	   		sourceList.at(sourceList.size()-1)->source->getProperties()->position->theta,
																	   		sourceList.at(sourceList.size()-1)->source->getProperties()->position->phi),
															   sourceList.at(sourceList.size()-1)->source->getProperties()->isLooping,
															   sourceList.at(sourceList.size()-1)->source->getProperties()->isVisible);
	  sourceList.at(sourceList.size()-1)->keyFrameList.push_back(new KeyFrame(p, 0.0));
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

std::vector<SoundSource*> AnimationPlayer::getSources(){
	std::vector<SoundSource*> returnList;
	for(std::vector<MasterSource*>::iterator source = sourceList.begin(); source != sourceList.end(); source++){
		SoundSource * temp = (*source)->source;
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

		//findClosestIndex
		//this search is O(n) aka kinda awful
		int index_lo = -1;
		int index_hi = -1;

		double time_lo = INFINITY;//(time_s - s->keyFrameList.at(0)->time_s);
		double time_hi = INFINITY;// abs(s->keyFrameList.at(0)->time_s - time_s);

		KeyFrame * kf;
		for( int i = 0; i < s->keyFrameList.size(); i++){
			kf = s->keyFrameList.at(i);

			if(abs(kf->time_s - time_s) < time_hi && (kf->time_s - time_s) >= 0){
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

	//step one
	double angleT = abs(lo->position->theta - hi->position->theta);
	if(angleT > 180){
		angleT = abs(angleT - 360);
	}
	//step two
	if(hi->position->theta < lo->position->theta){
		angleT *= -1;
	}
	double delta = lerp(0, angleT, time_s, time_lo, time_hi);
	//step three
	position->theta = fmod((lo->position->theta + 180 + delta), 360) - 180;

	position->phi = lerp(lo->position->phi, hi->position->phi, time_s, time_lo, time_hi);

	double scale = lerp(lo->scale, hi->scale, time_s, time_lo, time_hi);

	ssp = new SoundSourceProperties(position, lo->isLooping, lo->isVisible);
	ssp->scale = scale;
	return ssp;
}

double * AnimationPlayer::interpolateHRIR_linear(double index_a, double index_e, bool left, double * hrirLerped){
	int lower = (int)index_a;
	int upper = (int)index_a+1;
	if(upper > 26)
		upper = 26;
	for(int i = 0; i < 200; i++){
		if(left)
			hrirLerped[i] = lerp(hrir->hrir_l[lower][(int)index_e][i], hrir->hrir_l[upper][(int)index_e][i], index_a, lower, upper);
		else
			hrirLerped[i] = lerp(hrir->hrir_r[lower][(int)index_e][i], hrir->hrir_r[upper][(int)index_e][i], index_a, lower, upper);
	}
	return hrirLerped;
}

void AnimationPlayer::getBuffer(double ** buffer, double ** overflow, int frameStart, const int length){
	const int hrirLength = 200;
	const int convDataSize = length + hrirLength - 1;
	const int sourceChunkSize = length;
	double frameTime = frameStart/44100.0;

	double * audioData;

	mDataChunk = new double[sourceChunkSize];

	convDataL = new double[convDataSize];
	convDataR = new double[convDataSize];

	std::vector<SoundSource*> ssl = getSources(frameTime);

	for(int i =0; i< length; i++){
		buffer[0][i] = 0;
		buffer[1][i] = 0;
	}

	for(int i =0; i< hrirLength-1; i++){
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
		if((*source)->getProperties()->isVisible){
			for(int i = 0; i < sourceChunkSize; i++){
				if(start > frameTime){
					mDataChunk[i] = 0;
				}
				else{
					if((*source)->getProperties()->isLooping){
						double scale = (*source)->getProperties()->scale;
						mDataChunk[i] = scale*audioData[(i+frameStart)%(*source)->getLength()];
					} else {
						if(i + frameStart >= (*source)->getLength()){
							mDataChunk[i] = 0;
						} else{
							double scale = (*source)->getProperties()->scale;
							mDataChunk[i] = scale* audioData[i+frameStart];
						}
					}
				}
				frameTime += 1.0/44100.0;
				//delete (*source)->getProperties();
			}
			Polar3D *p = (*source)->getProperties()->position;

			double aziIndex = hrir->getIndices(p->theta, p->phi)[0];

			double eleIndex = hrir->getIndices(p->theta, p->phi)[1];

			interpolateHRIR_linear(aziIndex, eleIndex, true, hrirLL);
			convolve(mDataChunk, sourceChunkSize, hrirLL, 200, convDataL);

			interpolateHRIR_linear(aziIndex, eleIndex, false, hrirLR);
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
			}

			for(int i =length; i< length + 199; i++){
				overflow[0][i-length] += convDataL[i];
				overflow[1][i-length] += convDataR[i];
				if(abs(overflow[0][i-length]) > 1.0){
					overflow[0][i-length] = overflow[0][i-length]/abs(overflow[1][i-length]);
				}
				if(abs(overflow[1][i-length]) > 1.0){
					overflow[1][i-length] = overflow[1][i-length]/abs(overflow[1][i-length]);
				}
			}
		}
	}

	if(mDataChunk != NULL)
		delete[] mDataChunk;
	if(convDataL != NULL)
		delete[] convDataL;
	if(convDataR != NULL)
		delete[] convDataR;
}
