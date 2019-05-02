CC=gcc
CCV=
CFLAGS= -g -std=c++11
DEPS = HRIR_Data.h util.h AnimationPlayer.h SoundSource.h Track.h KeyFrame.h AudioPlayer.h interfacer.h
OBJ = util.o HRIR_Data.o Track.o SoundSource.o AnimationPlayer.o AudioPlayer.o KeyFrame.o interfacer.o main.o
LIBS= -lavformat -lavcodec -lswresample -lavutil -lportaudio -lmatio -lstdc++ -lm -lpthread
LIBS_T= -lavformat -lavcodec -lswresample -lavutil -lmatio -lstdc++ -lm

%.o: %.cpp $(DEPS)
	$(CC) -o $@ $(CCV) -c $< $(CFLAGS) -Wall -O0

../bin/audio_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -Wall -O0

rwar:
	$(CC) -o rwar main.cpp HRIR_Data.cpp util.cpp Track.cpp AnimationPlayer.cpp SoundSource.cpp KeyFrame.cpp $(CFLAGS) $(LIBS_T)

.PHONY: clean

clean:
	rm -f $(OBJ)
	g++ gtkmm_example/main.cpp util.cpp HRIR_Data.cpp Track.cpp SoundSource.cpp AnimationPlayer.cpp AudioPlayer.cpp KeyFrame.cpp interfacer.cpp  -o gtkmm_example/test `pkg-config gtkmm-3.0 --cflags --libs` -lavformat -lavcodec -lswresample -lavutil -lportaudio -lmatio -lstdc++ -lm -lpthread
