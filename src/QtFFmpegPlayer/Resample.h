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

	//输出参数和输入参数一致除了采样格式，输出为s16, 会释放para
	bool Open(AVCodecParameters *para);
	
	void Close();

	//返回重采样的大小 indata输入参数	data输出参数, 不管成功与否都释放indata
	int AudioResample(AVFrame* indata, unsigned char *outdata);

	//AV_SAMPLE_FMT_S16
	int outFormat = 1;

private:
	QMutex mutex;

	SwrContext *actx = NULL;
};

