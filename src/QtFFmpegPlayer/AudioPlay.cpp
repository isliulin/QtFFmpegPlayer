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

bool AudioPlay::Open(int sampleRate, int channels)
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

//�ر�QAudioOutput��QIODevice
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

//������Ƶ�������п��õĿ��пռ�(���ֽ�Ϊ��λ)��
int AudioPlay::GetFree()
{
	QMutexLocker locker(&mutex);
	if (!output) return 0;
	int free = output->bytesFree();
	return free;
}
//������д����Ƶ������
bool AudioPlay::Write(unsigned char* data, int dataSize)
{
	if (!data || data <= 0)return false;
	QMutexLocker locker(&mutex);
	if (!output || !io) return false;
	
	int size = io->write((char*)data, dataSize);
	locker.unlock();

	if (dataSize != size) return false;
	return true;
}
