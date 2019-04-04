CC=gcc
CFLAGS=
DEPS = HRIR_Data.h util.h AnimationPlayer.h SoundSource.h Track.h KeyFrame.h
OBJ = util.o HRIR_Data.o Track.o SoundSource.o AnimationPlayer.o AudioPlayer.o KeyFrame.o
LIBS= -lavformat -lavcodec -lswresample -lavutil -lportaudio -lmatio -lstdc++ -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

audio_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OBJ)
