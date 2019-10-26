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
//����������������һ�³��˲�����ʽ�����Ϊs16, ���۴������ͷ�para
bool Resample::Open(AVCodecParameters *para)
{
	if (!para) return false;
	QMutexLocker locker(&mutex);
	//���ظ�������ͨ����Ĭ��ͨ�����֡�
	//int x = av_get_default_channel_layout(2);
	actx = swr_alloc_set_opts(
		actx,
		av_get_default_channel_layout(para->channels),					//�����ʽ
		(AVSampleFormat)outFormat,							//���������ʽ
		para->sample_rate,									//���������
		av_get_default_channel_layout(para->channels),		//�����ʽ
		(AVSampleFormat)para->format,						//����������ʽ
		para->sample_rate,									//���������
		0, 0);
	avcodec_parameters_free(&para);

	locker.unlock();
	//����Ϊ�û��������ʼ��������
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

//�����ز����Ĵ�С indata�������	data�������, ���ܳɹ�����ͷ�indata
int Resample::AudioResample(AVFrame* indata, unsigned char *outdata)
{
	if (!indata) return 0;
	if (!outdata)
	{
		av_frame_free(&indata);
		return 0;
	}
	/*uint8_t *data = { 0 };
	data = outdata;*/
	int ret = swr_convert(actx, &outdata, indata->nb_samples,				//���
							(const uint8_t**)indata->data, indata->nb_samples	//����
	);
	
	if (ret <= 0) return ret;
	int size = ret * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
	av_frame_free(&indata);
	return size;
}

