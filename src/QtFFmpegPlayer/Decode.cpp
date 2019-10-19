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
	//查找解码器
	AVCodec *avcodec = avcodec_find_decoder(para->codec_id);
	if (!avcodec)
	{
		qDebug() << "can't find video AVCodec: id=" << para->codec_id;
		avcodec_parameters_free(&para);
		return false;
	}
	QMutexLocker locker(&mutex);
	//创建解码器上下文
	codec = avcodec_alloc_context3(avcodec);
	//复制解码器上下文参数
	avcodec_parameters_to_context(codec, para);
	//释放参数
	avcodec_parameters_free(&para);
	//代开解码器
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
//发送到解码线程，不管成功与否清理pkt空间（对象和媒体内容)
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
//获取解码后数据，一次send可能多次receive，最后获取缓冲中的数据send null在Recv多次
	//每次复制一份，由调用者释放 av_frame_free
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
	//释放编解码器上下文和所有与之相关的内容，并写入NULL提供的指针。
	avcodec_free_context(&codec);
}

void Decode::Close()
{
	QMutexLocker locker(&mutex);
	if (!codec) return;
	avcodec_close(codec);
	//释放编解码器上下文和所有与之相关的内容，并写入NULL提供的指针。
	avcodec_free_context(&codec);
}
