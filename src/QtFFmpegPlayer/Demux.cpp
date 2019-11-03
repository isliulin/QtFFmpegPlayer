#include "Demux.h"
#include <QMutexLocker>
#include <QDebug>
#include "PlayerUtility.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}


Demux::Demux()
{
	static bool isFrist = true;
	QMutexLocker locker(&mutex);
	if (isFrist)
	{
		//��ʼ����װ��
		av_register_all();
		//��ʼ�������
		avformat_network_init();
		isFrist = false;
	}
}


Demux::~Demux()
{
}

bool Demux::Open(const char* url)
{
	Close();
	AVDictionary *opts = NULL;
	//����rtsp����tcpЭ���
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	//������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);

	/*avdevice_register_all();

	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVInputFormat *ifmt = av_find_input_format("vfwcap");
	avformat_open_input(&pFormatCtx, 0, ifmt, NULL);
	//����
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVInputFormat *ifmt = av_find_input_format("dshow");
	avformat_open_input(&pFormatCtx, "video=Integrated Camera", ifmt, NULL);*/


	QMutexLocker locker(&mutex);
	int ret = avformat_open_input(&afc, url, NULL, &opts);
	if (!afc || afc->nb_streams < 2)
	{
		qDebug() << "not support this file! " << url;
		return false;
	}
	if (ret != 0)
	{
		PlayerUtility::Get()->av_strerror2(ret, url);
		return false;
	}
	//��ȡ����Ϣ
	ret = avformat_find_stream_info(afc, NULL);
	if (ret < 0)
	{
		PlayerUtility::Get()->av_strerror2(ret);
		return false;
	}
	totalMs = afc->duration / (AV_TIME_BASE / 1000);
	//��ӡ����ϸ��Ϣ
	av_dump_format(afc, 0, url, 0);

	qDebug() << "Total Ms:" << totalMs;
	qDebug() << "======================Video Infomation======================";
	videoStreamIndex = av_find_best_stream(afc, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	qDebug() << "video stream index:" << videoStreamIndex;
	qDebug() << "width:" << afc->streams[videoStreamIndex]->codecpar->width;
	qDebug() << "height:" << afc->streams[videoStreamIndex]->codecpar->height;
	//YUV420P YUV422֮��Ķ���
	qDebug() << "format:" << afc->streams[videoStreamIndex]->codecpar->format;
	//H264, H265 ֮��Ķ���
	qDebug() << "codec id:" << afc->streams[videoStreamIndex]->codecpar->codec_id;
	qDebug() << "fps:" << PlayerUtility::Get()->r2d(afc->streams[videoStreamIndex]->avg_frame_rate);
	
	qDebug() << "======================Audio Infomation======================";
	audioStreamIndex = av_find_best_stream(afc, AVMEDIA_TYPE_AUDIO, - 1, - 1, NULL, 0);
	qDebug() << "audio stream index:" << audioStreamIndex;
	qDebug() << "sample rate:" << afc->streams[audioStreamIndex]->codecpar->sample_rate;
	//FLTP, S16֮��Ķ���
	qDebug() << "format:" << afc->streams[audioStreamIndex]->codecpar->format;
	qDebug() << "channels:" << afc->streams[audioStreamIndex]->codecpar->channels;
	//AAC MP3 ֮��Ķ���
	qDebug() << "codec id:" << afc->streams[audioStreamIndex]->codecpar->codec_id;
	//һ֡���ݴ�ŵ���һ�������������� ��ͨ��������
	qDebug() << "frame size:" << afc->streams[audioStreamIndex]->codecpar->frame_size;
	qDebug() << "============================================================";

	qDebug() << "Open success";
	
	return true;
}

AVPacket * Demux::Read()
{
	QMutexLocker locker(&mutex);
	if (!afc) return NULL;
	AVPacket *pkt = av_packet_alloc();
	int ret = av_read_frame(afc, pkt);
	if (ret != 0)
	{
		av_packet_free(&pkt);
		return NULL;
	}
	//ת��ptsΪ����
	pkt->pts = pkt->pts * (PlayerUtility::Get()->r2d(afc->streams[pkt->stream_index]->time_base) * 1000);
	pkt->dts = pkt->dts * (PlayerUtility::Get()->r2d(afc->streams[pkt->stream_index]->time_base) * 1000);
	//qDebug() << "pts:" << pkt->pts;
	return pkt;
}

bool Demux::Seek(double pos)
{
	QMutexLocker locker(&mutex);
	if (!afc) return false;
	avformat_flush(afc);
	long long seekPos = 0;
	seekPos = afc->streams[videoStreamIndex]->duration * pos;
	int ret = av_seek_frame(afc, videoStreamIndex, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	if (ret < 0)
	{
		return false;
	}
	return true;
}

//��ȡ��Ƶ���� ���صĿռ���Ҫ���� avcodec_parameters_free
//Ŀǰֻ֧��AVMEDIA_TYPE_VIDEO��AVMEDIA_TYPE_AUDIO
AVCodecParameters* Demux::GetMediaParameters(AVMediaType type)
{
	QMutexLocker locker(&mutex);
	if (!afc) return NULL;
	AVCodecParameters *pa = avcodec_parameters_alloc();
	if (type == AVMEDIA_TYPE_VIDEO)
		avcodec_parameters_copy(pa, afc->streams[videoStreamIndex]->codecpar);
	else if (type == AVMEDIA_TYPE_AUDIO)
		avcodec_parameters_copy(pa, afc->streams[audioStreamIndex]->codecpar);
	else
		throw "current version not support this parameters";
	return pa;
}

bool Demux::IsVideo(AVPacket *pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == audioStreamIndex)
		return false;
	return true;
}

void Demux::Clear()
{
	QMutexLocker locker(&mutex);
	if (!afc) return;
	//�����ȡ����
	avformat_flush(afc);
}

void Demux::Close()
{
	QMutexLocker locker(&mutex);
	if (!afc) return;
	//�رմ򿪵�����AVFormatContext���ͷ����������������ݺ�����*afcΪ��
	avformat_close_input(&afc);
	totalMs = 0;
}
