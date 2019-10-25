#pragma once
#include <QMutex>
struct AVPacket;
struct AVFrame;
struct AVCodecParameters;
struct AVCodecContext;
class Decode
{
public:
	Decode();
	~Decode();

	//无论打开与否都释放 AVCodecParameters
	bool Open(AVCodecParameters *para);

	//发送到解码线程，不管成功与否清理pkt空间（对象和媒体内容)
	bool Send(AVPacket *pkt);
	//获取解码后数据，一次send可能多次receive，最后获取缓冲中的数据send null在Recv多次
	//每次复制一份，由调用者释放 av_frame_free
	AVFrame* Recv();

	//清理解码器上下文
	void Clear();
	//清理解码器上下文，并关闭，指针置为NULL
	void Close();
	
	long long pts = 0;
private:

	QMutex mutex;
	const int CODEC_THREAD_COUNT = 8;

	//解码器上下文，需要释放 avcodec_free_context
	AVCodecContext* codec = NULL;
};

