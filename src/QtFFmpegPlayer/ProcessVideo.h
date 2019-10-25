#pragma once
#include <QThread>
#include <QMutex>
#include "ProcessBase.h"
struct AVCodecParameters;
struct AVPacket;

class Decode;
class VideoCanvas;
class ProcessVideo : public ProcessBase
{

public:
	ProcessVideo();
	~ProcessVideo();
	//���۴�����ͷ�para
	virtual bool Open(AVCodecParameters* para);

	virtual void Push(AVPacket* pkt);

	void SetCanvas(VideoCanvas* canvas);
protected:
	void run();

private:
	VideoCanvas * canvas;


};

