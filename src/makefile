CC=g++
CFLAGS= -g
DEPS = HRIR_Data.h util.h AnimationPlayer.h SoundSource.h Track.h KeyFrame.h AudioPlayer.h
OBJ = util.o HRIR_Data.o Track.o SoundSource.o AnimationPlayer.o AudioPlayer.o KeyFrame.o main.o
LIBS= -lavformat -lavcodec -lswresample -lavutil -lportaudio -lmatio -lstdc++ -lm
LIBS_T= -lavformat -lavcodec -lswresample -lavutil -lmatio -lstdc++ -lm

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

../bin/audio_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

rwar:
	$(CC) -o rwar main.cpp HRIR_Data.cpp util.cpp Track.cpp AnimationPlayer.cpp SoundSource.cpp KeyFrame.cpp $(CFLAGS) $(LIBS_T)

.PHONY: clean

clean:
	rm -f $(OBJ)