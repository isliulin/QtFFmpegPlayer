#include "AudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QDebug>
AudioPlay::AudioPlay()
{
}


AudioPlay::~AudioPlay()
{
}

bool AudioPlay::Open()
{
	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setSampleSize(sampleSize);
	fmt.setChannelCount(channels);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);

	mutex.lock();
	output = new QAudioOutput(fmt);
	/*qDebug() << output->volume();
	qreal volume = 0.9;
	output->setVolume(volume);*/
	io = output->start();
	mutex.unlock();
	if (!io) return false;
	return true;
}

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

int AudioPlay::GetFree()
{
	QMutexLocker locker(&mutex);
	if (!output) return 0;
	int free = output->bytesFree();
	return free;
}

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
