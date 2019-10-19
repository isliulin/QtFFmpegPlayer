#pragma once
#include <QMutex>

extern "C"
{
#include <libavutil/avutil.h>
}

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class Demux
{
public:
	Demux();
	~Demux();

	//打开媒体文件， 流媒体(rtsp rtmp http)
	bool Open(const char* url);

	//空间需要调用者释放，释放AVPacket对象空间和数据空间， 使用av_packet_free
	AVPacket *Read();

	//seek位置 pos 0.0~1.0
	bool Seek(double pos);

	//获取视频参数 返回的空间需要清理 avcodec_parameters_free
	//目前只支持AVMEDIA_TYPE_VIDEO和AVMEDIA_TYPE_AUDIO
	AVCodecParameters* GetMediaParameters(AVMediaType type);

	//清空读取数据缓存
	void Clear();
	void Close();

public:

	int totalMs = 0;

private:
	AVFormatContext *afc = NULL;

	QMutex mutex;

	int videoStreamIndex = -1;
	int audioStreamIndex = -1;

};

