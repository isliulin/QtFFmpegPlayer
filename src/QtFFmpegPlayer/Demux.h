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

	//��ý���ļ��� ��ý��(rtsp rtmp http)
	bool Open(const char* url);

	//�ռ���Ҫ�������ͷţ��ͷ�AVPacket����ռ�����ݿռ䣬 ʹ��av_packet_free
	AVPacket *Read();

	//seekλ�� pos 0.0~1.0
	bool Seek(double pos);

	//��ȡ��Ƶ���� ���صĿռ���Ҫ���� avcodec_parameters_free
	//Ŀǰֻ֧��AVMEDIA_TYPE_VIDEO��AVMEDIA_TYPE_AUDIO
	AVCodecParameters* GetMediaParameters(AVMediaType type);

	//��ն�ȡ���ݻ���
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

