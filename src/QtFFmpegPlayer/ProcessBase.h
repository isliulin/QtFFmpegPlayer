#pragma once
#include <QThread>
#include <QMutex>
class Decode;
struct AVCodecParameters;
struct AVPacket;
class ProcessBase : public QThread
{
public:
	ProcessBase();
	~ProcessBase();

	//无论打开与否都释放para
	virtual bool Open(AVCodecParameters* para) = 0;

	void Push(AVPacket* pkt);

protected:
	Decode* decode = NULL;

	std::list<AVPacket*> packets;
	std::list<AVPacket*> tmpPkts;

	QMutex mutex;
	QMutex tmpMtx;

	bool isExist = false;
};

