#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <sstream>
#include <fstream>

#include"Track.h"
#include"util.h"
#include"AudioPlayer.h"

#include"AnimationPlayer.h"
#include"interfacer.h"


int main(int argc,  char * argv[])
{
	Interfacer * ifr = new Interfacer();
	ifr->setInStream(&std::cin);
	std::thread(&Interfacer::myMain, ifr).join();

	return 0;

}
