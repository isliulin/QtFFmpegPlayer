#include "Player_1_0.h"
#include "Demux.h"
#include "VideoCanvas.h"
#include "ProcessAudio.h"
#include "ProcessVideo.h"
#include <QDebug>
#include "PlayerUtility.h"
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

Player_1_0::Player_1_0()
{
	demux = new Demux();
	audio = new ProcessAudio();
	video = new ProcessVideo();
}


Player_1_0::~Player_1_0()
{
	isExit = true;
	delete audio;
	audio = NULL;
	delete video;
	video = NULL;
	wait();
	qDebug() << "Close Player 1.0";
}

bool Player_1_0::Open(VideoCanvas * canvas, const char* path)
{
	video->SetCanvas(canvas);
	if (!demux->Open(path))
		return false;

	if (!video->Open(demux->GetMediaParameters(AVMEDIA_TYPE_VIDEO))) return false;
	video->start();

	if (!audio->Open(demux->GetMediaParameters(AVMEDIA_TYPE_AUDIO))) return false;
	audio->start();
	return true;
}

void Player_1_0::GetVideoSize(int *width, int *height)
{
	AVCodecParameters * pa = demux->GetMediaParameters(AVMEDIA_TYPE_VIDEO);
	*width = pa->width;
	*height = pa->height;
	avcodec_parameters_free(&pa);
}


void Player_1_0::run()
{
	while (!isExit)
	{

		if (PlayerUtility::Get()->isPause)
		{
			QThread::msleep(1);
			continue;
		}
		AVPacket *pkt = demux->Read();
		if (pkt == NULL)
		{
			//demux->Seek(0);
			qDebug() << "decode ok";
			break;
		}
		if (demux->IsVideo(pkt))
		{
			/*if(video)
				video->Push(pkt);*/
			av_packet_free(&pkt);
		}
		else
		{
			if(audio)
				audio->Push(pkt);
		}
		QThread::msleep(1);
	}
	qDebug() << QThread::currentThreadId() << "Demux Thread Quit";
}
