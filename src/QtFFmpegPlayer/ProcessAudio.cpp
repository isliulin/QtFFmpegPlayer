#include "ProcessAudio.h"
#include "Decode.h"
#include "Resample.h"
#include "AudioPlay.h"
#include <QDebug>
#include <QMutexLocker>
#include <QtConcurrent/QtConcurrent>
#include "PlayerUtility.h"
extern"C"
{
#include <libavcodec/avcodec.h>
}

struct AudioData
{
	unsigned char* data;
	int length;
	AudioData(int len)
	{
		data = new unsigned char[len];
		length = len;
	}
	void Drop()
	{
		delete data;
		data = NULL;
	}
};

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

	//QtConcurrent::run(this, &ProcessAudio::PlayAudioThread);
	return isSuccess;

}

void ProcessAudio::Push(AVPacket* pkt)
{
	if (!pkt) return;
	while (!isExist && packets.size() > 100)
	{
		QThread::msleep(1);
	}
	QMutexLocker locker(&tmpMtx);
	tmpPkts.push_back(pkt);
	//qDebug() << "add packet to tmpPkts:" << packets.size();

}

void ProcessAudio::run()
{
	bool isWritePCM2file = false;
	FILE* fp = NULL;
	if (isWritePCM2file)
	{
		
		fp = fopen("C:/Users/Administrator/Desktop/tmp/4k.pcm", "wb");
		if (!fp)
		{
			qDebug() << "file open failed";
			return;
		}
	}
	
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
	while (!isExist)
	{
		if (PlayerUtility::Get()->isPause)
		{
			QThread::msleep(1);
			continue;
		}

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
		//qDebug() << "audio packets size:" << packets.size();
		while (!isExist)
		{
			AVFrame* frame = decode->Recv();
			if (!frame) break;
			//处理音频帧 重采样

			long long recordTime = PlayerUtility::Get()->GetNowMs();
			PlayerUtility::Get()->audioPts = decode->pts - aplay->GetNoPlayMs();
			
			int len = resample->AudioResample(frame, pcm);

			while (len > 0)
			{
				if (aplay->GetFree() >= len)
				{
					PlayerUtility::Get()->justWritePts = decode->pts + (PlayerUtility::Get()->GetNowMs() - recordTime);
					aplay->Write(pcm, len);
					if (isWritePCM2file)
					{
						fwrite(pcm, len, 1, fp);
					}

					break;
				}
				QThread::msleep(1);
			}
		}

	}
	if (isWritePCM2file)
	{
		fclose(fp);
	}
	delete pcm;
	pcm = NULL;
	qDebug() << QThread::currentThreadId() << "Process Audio Thread Quit";
}

void ProcessAudio::PlayAudioThread()
{
	while (true)
	{
		if (adlist.size() == 0)
		{
			QThread::msleep(1);
			continue;
		}
		AudioData* ad = adlist.front();
		adlist.pop_front();
		while (true)
		{
			if (aplay->GetFree() >= ad->length)
			{
				aplay->Write(ad->data, ad->length);
				//fwrite(pcm, len, 1, fp);
				ad->Drop();
				break;
			}
			QThread::msleep(1);
		}
		QThread::msleep(1);
		qDebug() << adlist.size();
	}
}

