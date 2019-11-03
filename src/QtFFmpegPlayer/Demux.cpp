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
		//初始化封装库
		av_register_all();
		//初始化网络库
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
	//设置rtsp流以tcp协议打开
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	//网络延时时间
	av_dict_set(&opts, "max_delay", "500", 0);

	/*avdevice_register_all();

	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVInputFormat *ifmt = av_find_input_format("vfwcap");
	avformat_open_input(&pFormatCtx, 0, ifmt, NULL);
	//或者
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
	//获取流信息
	ret = avformat_find_stream_info(afc, NULL);
	if (ret < 0)
	{
		PlayerUtility::Get()->av_strerror2(ret);
		return false;
	}
	totalMs = afc->duration / (AV_TIME_BASE / 1000);
	//打印流详细信息
	av_dump_format(afc, 0, url, 0);

	qDebug() << "Total Ms:" << totalMs;
	qDebug() << "======================Video Infomation======================";
	videoStreamIndex = av_find_best_stream(afc, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	qDebug() << "video stream index:" << videoStreamIndex;
	qDebug() << "width:" << afc->streams[videoStreamIndex]->codecpar->width;
	qDebug() << "height:" << afc->streams[videoStreamIndex]->codecpar->height;
	//YUV420P YUV422之类的东西
	qDebug() << "format:" << afc->streams[videoStreamIndex]->codecpar->format;
	//H264, H265 之类的东西
	qDebug() << "codec id:" << afc->streams[videoStreamIndex]->codecpar->codec_id;
	qDebug() << "fps:" << PlayerUtility::Get()->r2d(afc->streams[videoStreamIndex]->avg_frame_rate);
	
	qDebug() << "======================Audio Infomation======================";
	audioStreamIndex = av_find_best_stream(afc, AVMEDIA_TYPE_AUDIO, - 1, - 1, NULL, 0);
	qDebug() << "audio stream index:" << audioStreamIndex;
	qDebug() << "sample rate:" << afc->streams[audioStreamIndex]->codecpar->sample_rate;
	//FLTP, S16之类的东西
	qDebug() << "format:" << afc->streams[audioStreamIndex]->codecpar->format;
	qDebug() << "channels:" << afc->streams[audioStreamIndex]->codecpar->channels;
	//AAC MP3 之类的东西
	qDebug() << "codec id:" << afc->streams[audioStreamIndex]->codecpar->codec_id;
	//一帧数据存放的是一定数量的样本数 单通道样本数
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
	//转换pts为毫秒
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

//获取视频参数 返回的空间需要清理 avcodec_parameters_free
//目前只支持AVMEDIA_TYPE_VIDEO和AVMEDIA_TYPE_AUDIO
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
	//清理读取缓冲
	avformat_flush(afc);
}

void Demux::Close()
{
	QMutexLocker locker(&mutex);
	if (!afc) return;
	//关闭打开的输入AVFormatContext。释放它和它的所有内容和设置*afc为空
	avformat_close_input(&afc);
	totalMs = 0;
}
