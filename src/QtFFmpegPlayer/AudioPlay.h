#pragma once
#include <QMutex>
class QAudioOutput;
class QIODevice;
class AudioPlay
{
public:
	AudioPlay();
	~AudioPlay();

public:
	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;

	bool Open();
	void Close();

	int GetFree();

	bool Write(char* data, int dataSize);

private:
	QMutex mutex;
	QAudioOutput *output = NULL;
	QIODevice * io = NULL;
};

