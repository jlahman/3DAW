# 3DAW

## Building:
### Dependencies:

In order to build this project, the following system libraries need to be installed:
* [Matio](https://github.com/tbeu/matio)
* [PortAudio](http://www.portaudio.com/)
* [FFMPEG libraries](https://github.com/FFmpeg/FFmpeg):
	* [avcodec]()
	* [avformat]()
	* [avutil]()
	* [swresample]()
	
If you are using a linux distro, your software repository probably has these. On ubuntu 18.04 they can be installed by:
`sudo apt install libmatio-dev libportaudio2 libavcodec-dev libavformat-dev libavutil-dev libavresample-dev`

### Using CMAKE:
1. Download Project and extract to desired location
1. Change directory to '~/3DAW'
1. Make a directory named 'build'
1. Change director to 'build'
1. Run: 'cmake ../src'
1. install according to your platforms default way
	* i.e. on systems with `make` just run `make`
1. Run the executable `~/3DAW/bin/audio_test`
	
### Using make:
run in src directory: 
`make audio_test`
`make clean`

run in bin directory: ./audio_test

## Running:
On successful building:
Run the executable `~/3DAW/bin/audio_test`

