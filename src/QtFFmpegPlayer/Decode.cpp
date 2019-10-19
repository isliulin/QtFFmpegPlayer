#include "Decode.h"
#include <QDebug>
#include <QMutexLocker>
#include "PlayerUtility.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}

Decode::Decode()
{
}


Decode::~Decode()
{
}

bool Decode::Open(AVCodecParameters *para)
{
	if (!para) return false;
	Close();
	//���ҽ�����
	AVCodec *avcodec = avcodec_find_decoder(para->codec_id);
	if (!avcodec)
	{
		qDebug() << "can't find video AVCodec: id=" << para->codec_id;
		avcodec_parameters_free(&para);
		return false;
	}
	QMutexLocker locker(&mutex);
	//����������������
	codec = avcodec_alloc_context3(avcodec);
	//���ƽ����������Ĳ���
	avcodec_parameters_to_context(codec, para);
	//�ͷŲ���
	avcodec_parameters_free(&para);
	//����������
	codec->thread_count = CODEC_THREAD_COUNT;
	int ret = avcodec_open2(codec, NULL, NULL);
	if (ret != 0)
	{
		PlayerUtility::Get()->av_strerror2(ret);
		return false;
	}
	qDebug() << "open codec success!";
	return true;
}
//���͵������̣߳����ܳɹ��������pkt�ռ䣨�����ý������)
bool Decode::Send(AVPacket *pkt)
{
	if (!pkt || pkt->size <= 0 || !pkt->data) return false;
	QMutexLocker locker(&mutex);
	if (!codec) return false;
	int ret = avcodec_send_packet(codec, pkt);
	locker.unlock();
	av_packet_free(&pkt);
	if (ret != 0) return false;
	return true;
}
//��ȡ��������ݣ�һ��send���ܶ��receive������ȡ�����е�����send null��Recv���
	//ÿ�θ���һ�ݣ��ɵ������ͷ� av_frame_free
AVFrame* Decode::Recv()
{
	QMutexLocker locker(&mutex);
	if (!codec) return NULL;
	AVFrame *frame = av_frame_alloc();
	int ret = avcodec_receive_frame(codec, frame);
	locker.unlock();
	if (ret != 0)
	{
		av_frame_free(&frame);
		return NULL;
	}
	//qDebug() << frame->pts;
	return frame;
}

void Decode::Clear()
{
	QMutexLocker locker(&mutex);
	if (!codec) return;
	//�ͷű�����������ĺ�������֮��ص����ݣ���д��NULL�ṩ��ָ�롣
	avcodec_free_context(&codec);
}

void Decode::Close()
{
	QMutexLocker locker(&mutex);
	if (!codec) return;
	avcodec_close(codec);
	//�ͷű�����������ĺ�������֮��ص����ݣ���д��NULL�ṩ��ָ�롣
	avcodec_free_context(&codec);
}
