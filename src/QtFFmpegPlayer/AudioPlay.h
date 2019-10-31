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

	//�ر�QAudioOutput��QIODevice
	void Close();

	//������Ƶ�������п��õĿ��пռ�(���ֽ�Ϊ��λ)��
	int GetFree();

	int GetPeriodSize();

	//������д����Ƶ������
	bool Write(unsigned char* data, int dataSize);
	
	//���ػ�δ���ŵ�ʱ��ms
	long long GetNoPlayMs();
private:
	QMutex mutex;
	QAudioOutput *output = NULL;
	QIODevice * io = NULL;
};

