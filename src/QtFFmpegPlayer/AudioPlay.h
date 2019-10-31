#pragma once
#include <QMutex>
class QAudioOutput;
class QIODevice;
class AVCodecParameters;
class AudioPlay
{
public:
	AudioPlay();
	~AudioPlay();

public:
	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;

	bool Open(int sampleRate, int channels);

	//关闭QAudioOutput和QIODevice
	void Close();

	//返回音频缓冲区中可用的空闲空间(以字节为单位)。
	int GetFree();

	int GetPeriodSize();

	//把数据写入音频缓冲区
	bool Write(unsigned char* data, int dataSize);
	
	//返回还未播放的时间ms
	long long GetNoPlayMs();
private:
	QMutex mutex;
	QAudioOutput *output = NULL;
	QIODevice * io = NULL;
};

