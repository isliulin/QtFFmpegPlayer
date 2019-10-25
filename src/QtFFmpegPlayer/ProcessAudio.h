#pragma once
#include "ProcessBase.h"
#include <QMutex>
class Decode;
class Resample;
class AudioPlay;
struct AVCodecParameters;
struct AVPacket;
class ProcessAudio : public ProcessBase
{
public:
	ProcessAudio();
	~ProcessAudio();

	//���۴�����ͷ�para
	virtual bool Open(AVCodecParameters* para);

protected:
	void run();


private:
	Resample* resample = NULL;
	AudioPlay* aplay = NULL;

};
