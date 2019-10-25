#include "ProcessAudio.h"
#include "Decode.h"
#include "Resample.h"
#include "AudioPlay.h"
#include <QDebug>
#include <QMutexLocker>
#include "PlayerUtility.h"
extern"C"
{
#include <libavcodec/avcodec.h>
}


ProcessAudio::ProcessAudio()
{
	decode = new Decode();
	resample = new Resample();
	aplay = new AudioPlay();

}



ProcessAudio::~ProcessAudio()
{
	isExist = true;
	decode->Close();
	wait();
}
//无论打开与否都释放para
bool ProcessAudio::Open(AVCodecParameters* para)
{

	if (!aplay->Open(para->sample_rate, para->channels))
	{
		avcodec_parameters_free(&para);
		return false;
	}
	AVCodecParameters* para2 = avcodec_parameters_alloc();
	avcodec_parameters_copy(para2, para);

	bool isSuccess = true;
	isSuccess = decode->Open(para);
	isSuccess = resample->Open(para2);

	return isSuccess;

}

void ProcessAudio::run()
{
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
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
		if (packets.empty() || !decode || !resample || !aplay)
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
			//处理音频帧 重采样

			int len = resample->AudioResample(frame, pcm);
			while (len > 0)
			{
				if (aplay->GetFree() >= len)
				{
					aplay->Write(pcm, len);
					break;
				}
				QThread::msleep(1);
			}
		}

	}
	delete pcm;
	pcm = NULL;
}

