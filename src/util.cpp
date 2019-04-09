#include"util.h"

extern "C" {
  #include<libavutil/opt.h>
  #include<libavformat/avformat.h>
  #include<libavcodec/avcodec.h>
  #include <libswresample/swresample.h>
}

void convolve( double* Signal, uint32_t SignalLen, double* Kernel, uint32_t KernelLen, double * output)
{
  uint32_t n;
  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    uint32_t kmin, kmax, k;

    output[n] = 0;

    kmin = (n >= SignalLen - 1) ? n - (SignalLen - 1) : 0;
    kmax = (n < KernelLen - 1) ? n : KernelLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      output[n] += Signal[n - k] * Kernel[k];
    }

    output[n] = output[n];
    if(output[n] > 1.0){
      output[n] = 1.0;

    }
    else if( output[n] < -1.0) {
      output[n] = -1.0;
    }
  }
}

int loadAudioData(std::string filepath, int sampleRate, int channels, double ** data, int *length){
   int * size = length;//(int*)malloc(sizeof(int));

   std::string prefix = "file:";
   prefix = prefix + filepath;
   const char * url = prefix.c_str();
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
   \

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


   AVFrame * pFrame = av_frame_alloc();

   AVPacket * pPacket = av_packet_alloc();

   int response = 0;

   *data = NULL;

   *size = 0;

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
         //fprintf(stderr, "hewwo\n" );
         //get the next frame from the codec context and store ret val in response
         response = avcodec_receive_frame(pCodecContext, pFrame);
         //printf("response: %d\n", response);

         if( response == AVERROR(EAGAIN) ||response == AVERROR_EOF){
           //do something else other than break i think

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
           *data = (double*) realloc(*data, (*size + pFrame->nb_samples) * sizeof(double));
           memcpy(*data + *size, buffer, frame_count * sizeof(double));
           *size += frame_count;

         }
         av_frame_unref(pFrame);
       }

     }
     av_packet_unref(pPacket);
   }
}

double lerp(double y0, double y1, double x, double x0, double x1){
  double interval = x1 - x0;
  double portionY0 = x1 - x;
  double portionY1 = x - x0;

  return (y0*portionY0 + y1*portionY1)/interval;
}
