#include"Tract.h"
#include <stdio.h>
#include <stdlib.h>

extern "C" {
  #include<libavutil/opt.h>
  #include<libavformat/avformat.h>
  #include<libavcodec/avcodec.h>
  #include <libswresample/swresample.h>
}



Tract::Tract(std::string filepath){
  filepath = filepath;
  sampleRate = 44100;
  azimuth = 0.0;
  loadProperties();
}

int Tract::getLength(){
  return length;
}

int Tract::getSampleRate(){
  return sampleRate;
}

double ** Tract::getData(){
  return &data;
}

double Tract::getAzimuth(){
  return azimuth;
}

void Tract::setAzimuth(double azimuth){
  azimuth = azimuth;
}

int Tract::loadProperties(){
  //double* data;// data = malloc(2*sizeof(double));

  int * size = (int*)malloc(sizeof(int));
  const char * url = "file:test.wav";
  //register all av calls?
  av_register_all();

  //open input file and allocate fromat context
  AVFormatContext * pFormatContext = NULL;
  if(avformat_open_input(&pFormatContext, url, NULL, NULL) != 0){
    fprintf(stderr, "ERROR: couldn't open input file!\n");
  }
  //retrieve the audio stream info, used for audio formats that don't have a header
  if(avformat_find_stream_info(pFormatContext, NULL) != 0){
    fprintf(stderr, "ERROR: couldn't find stream info\n");
  }

  fprintf(stderr, "DEBUG1: \n");


  //prepare codec Context
  AVCodec * pCodec = NULL;
  AVCodecParameters * pCodecParameters = NULL;
  int audio_stream_index = -1;
  for(int i = 0; i < pFormatContext->nb_streams ; i++){
    pCodecParameters = pFormatContext->streams[i]->codecpar;
    if(pCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO){
      audio_stream_index = i;
      pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
      break;
    }
  }
  fprintf(stderr, "DEBUG2: \n");

  AVCodecContext * pCodecContext = avcodec_alloc_context3(pCodec);
  if(!pCodecContext){
    fprintf(stderr, "ERROR: couldn't allocate memore for AVCodecContext\n");
    //quit
  }
  if(avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0){
    //error
    fprintf(stderr, "ERROR: couldn't set parameters AVCodecContext\n");

  }

  if(avcodec_open2(pCodecContext, pCodec, NULL) < 0){
    //error
    fprintf(stderr, "ERROR: couldn't  open codec for AVCodecContext\n");

  }


  AVFrame * pFrame = av_frame_alloc();  fprintf(stderr, "DEBUG2: \n");

  AVPacket * pPacket = av_packet_alloc();  fprintf(stderr, "DEBUG2: \n");

  int response = 0;

  data = NULL;  fprintf(stderr, "DEBUG4: \n");

  *size = 0;
  fprintf(stderr, "DEBUG3: \n");

  // prepare resampler
   SwrContext* swr = swr_alloc();
  av_opt_set_int(swr, "in_channel_count",  pCodecParameters->channels, 0);
  av_opt_set_int(swr, "out_channel_count", 1, 0);
  av_opt_set_int(swr, "in_channel_layout",  pCodecParameters->channel_layout, 0);
  av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
  av_opt_set_int(swr, "in_sample_rate", pCodecParameters->sample_rate, 0);
  av_opt_set_int(swr, "out_sample_rate", 44100, 0);
  av_opt_set_sample_fmt(swr, "in_sample_fmt",  pCodec->sample_fmts[0], 0);
  av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_DBL,  0);
  swr_init(swr);
  if (!swr_is_initialized(swr)) {
      fprintf(stderr, "Resampler has not been properly initialized\n");
      return -1;
  }



  while(av_read_frame(pFormatContext, pPacket) >= 0){
    if(pPacket->stream_index == audio_stream_index){
      //decode packet logic
      //store frame data in data struct
      response = avcodec_send_packet(pCodecContext, pPacket);
      if(response < 0){
        fprintf(stderr, "ERROR sending packet\n");
        //return response
        break;
      }
      //decode all frames in the packet
      while(response >= 0){
        fprintf(stderr, "hewwo\n" );
        //get the next frame from the codec context and store ret val in response
        response = avcodec_receive_frame(pCodecContext, pFrame);
        printf("response: %d\n", response);

        if( response == AVERROR(EAGAIN) ||response == AVERROR_EOF){   printf("ERRORS\n");

          break;
        } else if (response < 0){
          fprintf(stderr, "ERROR decoding frame\n" );
          return -1;//response;

        }
        //if a frame was successfully returned
        if(response >= 0){
          //write to data
          double* buffer;
          av_samples_alloc((uint8_t**) &buffer, NULL, 1, pFrame->nb_samples, AV_SAMPLE_FMT_DBL, 0);
          int frame_count = swr_convert(swr, (uint8_t**) &buffer, pFrame->nb_samples, (const uint8_t**) pFrame->data, pFrame->nb_samples);
          // append resampled frames to data
          data = (double*) realloc(data, (*size + pFrame->nb_samples) * sizeof(double));
          memcpy(data + *size, buffer, frame_count * sizeof(double));
          *size += frame_count;

        }
        av_frame_unref(pFrame);
      }

    }
    av_packet_unref(pPacket);
  }
  length = *size;
}
