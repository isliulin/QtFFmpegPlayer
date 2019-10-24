#pragma once
#include <QThread>
class Decode;
class Resample;
class AudioPlay;
struct AVCodecParameters;
class ProcessAudio : public QThread
{
public:
	ProcessAudio();
	~ProcessAudio();

	//���۴�����ͷ�para
	bool Open(AVCodecParameters* para);

protected:
	void run();

private:
	Decode* decode = NULL;
	Resample* resample = NULL;
	AudioPlay* aplay = NULL;
};

