#include "ProcessVideo.h"
#include <QMutexLocker>
#include "VideoCanvas.h"
#include "Decode.h"
#include "PlayerUtility.h"
extern"C"
{
#include <libavcodec/avcodec.h>
}

ProcessVideo::ProcessVideo()
{
	decode = new Decode();
}


ProcessVideo::~ProcessVideo()
{
	isExist = true;
	decode->Close();
	wait();
}

bool ProcessVideo::Open(AVCodecParameters* para)
{
	if (!this->canvas)
	{
		qDebug() << "Video Canvas is NULL";
		return false;
	}
	this->canvas->Init(para->width, para->height);
	if (!decode->Open(para)) return false;
	return true;
}


void ProcessVideo::SetCanvas(VideoCanvas* canvas)
{
	this->canvas = canvas;
}

void ProcessVideo::run()
{
	while (!isExist)
	{
		tmpMtx.lock();
		int size = tmpPkts.size();
		for (int i = 0; i < size; i++)
		{
			packets.push_back(tmpPkts.front());
			tmpPkts.pop_front();
		}
		tmpMtx.unlock();
		QMutexLocker locker(&mutex);
		if (packets.empty() || !decode || !canvas)
		{
			QThread::msleep(1);
			continue;
		}
		AVPacket *pkt = packets.front();
		packets.pop_front();
		bool ret = decode->Send(pkt);

		if (!ret)
		{
			QThread::msleep(1);
			continue;
		}
		while (!isExist)
		{
			AVFrame* frame = decode->Recv();
			if (!frame) break;

			canvas->Repaint(frame);
		}
	}
}
