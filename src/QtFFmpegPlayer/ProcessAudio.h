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

	//无论打开与否都释放para
	bool Open(AVCodecParameters* para);

protected:
	void run();

private:
	Decode* decode = NULL;
	Resample* resample = NULL;
	AudioPlay* aplay = NULL;
};

