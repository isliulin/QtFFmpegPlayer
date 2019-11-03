#include "AudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QDebug>
#include "PlayerUtility.h"
AudioPlay::AudioPlay()
{
	PlayerUtility::Get()->ap = this;
}


AudioPlay::~AudioPlay()
{
}

bool AudioPlay::Open(int sampleRate, int channels)
{
	this->channels = channels;
	this->sampleRate = sampleRate;
	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setSampleSize(sampleSize);
	fmt.setChannelCount(channels);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::SignedInt);

	mutex.lock();
	output = new QAudioOutput(fmt);
	/*qDebug() << output->volume();
	qreal volume = 0.9;
	output->setVolume(volume);*/
	io = output->start();
	//qDebug() << "buffsize:" << output->bufferSize(); //19200
	//qDebug() << "periodsize:" << output->periodSize();//3840
	//qDebug() << "byteFree:" << output->bytesFree();
	mutex.unlock();
	if (!io) return false;
	return true;
}

//关闭QAudioOutput和QIODevice
void AudioPlay::Close()
{
	mutex.lock();
	if (io)
	{
		io->close();
		io = NULL;
	}
	if (output)
	{
		output->stop();
		delete output;
		output = NULL;
	}
}

//返回音频缓冲区中可用的空闲空间(以字节为单位)。
int AudioPlay::GetFree()
{
	QMutexLocker locker(&mutex);
	if (!output) return 0;
	int free = output->bytesFree();
	return free;
}

int AudioPlay::GetPeriodSize()
{
	QMutexLocker locker(&mutex);
	if (!output) return 0;
	int size = output->periodSize();
	return size;
}

//把数据写入音频缓冲区
bool AudioPlay::Write(char* data, int dataSize)
{
	if (!data || data <= 0)return false;
	QMutexLocker locker(&mutex);
	if (!output || !io) return false;
	int size = io->write((char*)data, dataSize);
	locker.unlock();

	if (dataSize != size) return false;

	return true;
}

//返回还未播放的时间ms
long long AudioPlay::GetNoPlayMs()
{
	QMutexLocker locker(&mutex);
	if (!output) return 0;
	long long ms = 0;
	//还未播放的字节数
	double size = output->bufferSize() - output->bytesFree();
	//1秒音频字节大小
	double secSize = sampleRate * (sampleSize / 8) * channels;
	if (secSize <= 0) ms = 0;
	else ms = (size / secSize) * 1000;
	return ms;
}

long long AudioPlay::GetPeriodMs()
{
	QMutexLocker locker(&mutex);
	if (!output) return 0;
	long long ms = 0;
	//还未播放的字节数
	double size = output->periodSize();
	//1秒音频字节大小
	double secSize = sampleRate * (sampleSize / 8) * channels;
	if (secSize <= 0) ms = 0;
	else ms = (size / secSize) * 1000;
	return ms;
}
