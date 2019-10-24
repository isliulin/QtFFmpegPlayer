#include "ProcessAudio.h"
#include "Decode.h"
#include "Resample.h"
#include "AudioPlay.h"
extern"C"
{
#include <libavcodec/avcodec.h>
}

ProcessAudio::ProcessAudio()
{
	decode = new Decode();
	resample = new Resample();
	aplay = new AudioPlay();
}


ProcessAudio::~ProcessAudio()
{
}
//无论打开与否都释放para
bool ProcessAudio::Open(AVCodecParameters* para)
{
	
	if (!aplay->Open(para->sample_rate, para->channels))
	{
		avcodec_parameters_free(&para);
		return false;
	}
	AVCodecParameters* para2 = avcodec_parameters_alloc();
	avcodec_parameters_copy(para2, para);

	bool isSuccess = true;
	isSuccess = decode->Open(para);
	isSuccess = resample->Open(para2);

	avcodec_parameters_free(&para);
	avcodec_parameters_free(&para2);

	return isSuccess;

}

void ProcessAudio::run()
{

}
