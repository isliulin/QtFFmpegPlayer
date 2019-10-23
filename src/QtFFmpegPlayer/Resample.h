#pragma once
#include <QMutex>

struct AVCodecParameters;
struct SwrContext;
struct AVFrame;
class Resample
{
public:
	Resample();
	~Resample();

	//����������������һ�³��˲�����ʽ�����Ϊs16, ���ͷ�para
	bool Open(AVCodecParameters *para);
	
	void Close();

	//�����ز����Ĵ�С indata�������	data�������, ���ܳɹ�����ͷ�indata
	int AudioResample(AVFrame* indata, unsigned char *outdata);

	//AV_SAMPLE_FMT_S16
	int outFormat = 1;

private:
	QMutex mutex;

	SwrContext *actx = NULL;
};

