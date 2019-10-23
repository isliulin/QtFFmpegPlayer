#include "Resample.h"
#include <QMutexLocker>
#include "PlayerUtility.h"
#include <QDebug>
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

Resample::Resample()
{
}


Resample::~Resample()
{
}

bool Resample::Open(AVCodecParameters *para)
{
	if (!para) return false;
	QMutexLocker locker(&mutex);

	int x = av_get_default_channel_layout(2);
	actx = swr_alloc_set_opts(
		actx,
		av_get_default_channel_layout(2),					//输出格式
		(AVSampleFormat)outFormat,							//输出样本格式
		para->sample_rate,									//输出采样率
		av_get_default_channel_layout(para->channels),		//输入格式
		(AVSampleFormat)para->format,						//输入样本格式
		para->sample_rate,									//输入采样率
		0, 0);
	avcodec_parameters_free(&para);

	locker.unlock();
	int ret = swr_init(actx);
	if (ret != 0)
	{
		PlayerUtility::Get()->av_strerror2(ret);
		qDebug() << "swr_init failed";
		return false;
	}
	return true;
}

void Resample::Close()
{
	QMutexLocker locker(&mutex);
	if (actx) swr_free(&actx);
}

int Resample::AudioResample(AVFrame* indata, unsigned char *outdata)
{
	if (!indata) return 0;
	if (!outdata)
	{
		av_frame_free(&indata);
		return 0;
	}
	uint8_t *data[2] = { 0 };
	data[0] = outdata;
	int ret = swr_convert(actx, data, indata->nb_samples,				//输出
							(const uint8_t**)indata->data, indata->nb_samples	//输出
	);
	
	if (ret <= 0) return ret;
	int size = ret * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
	av_frame_free(&indata);
	return size;
}

