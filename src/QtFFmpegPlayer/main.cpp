#include "QtFFmpegPlayer.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QThread>
#include "Demux.h"
#include "Decode.h"
#include "VideoCanvas.h"
#include "VideoCanvas2.h"
#include "Resample.h"
#include "AudioPlay.h"

extern "C"
{
#include <libavformat/avformat.h>
}
#pragma  comment(lib, "avformat.lib")
#pragma  comment(lib, "avcodec.lib")
#pragma  comment(lib, "avutil.lib")
#pragma  comment(lib, "swresample.lib")

class Test : public QThread
{

public:
	Demux demux;
	Decode adecode;
	Decode vdecode;
	VideoCanvas* video;
	Resample resample;
	AudioPlay aplay;
	bool isExist = false;
	bool Init(const char* path = NULL)
	{
		qDebug() << avformat_configuration();
		//qDebug() <<  avformat_version(); //3756900
		//qDebug() << avcodec_version(); //3763044
		bool isOpenSuccess = false;
		if (path) isOpenSuccess = demux.Open(path);
		else
		{
			isOpenSuccess = demux.Open("F:/HTTP/Faded.mp4");
			isOpenSuccess = demux.Open("F:/硕鼠下载/Dragon PigCNBALLERCloud Wang.mp4");
			//isOpenSuccess = demux.Open("C:/Users/Administrator/Desktop/dl/nfdw.mp4");
			//isOpenSuccess = demux.Open("C:/Users/Administrator/Desktop/dl/8K机房监控.mp4");
		}
		//demux.Seek(0.5);
		if (!isOpenSuccess)
		{
			qDebug() << "open failed!";
			return false;
		}
		
		vdecode.Open(demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO));
		adecode.Open(demux.GetMediaParameters(AVMEDIA_TYPE_AUDIO));
		video->Init(demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO)->width, demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO)->height);
		resample.Open(demux.GetMediaParameters(AVMEDIA_TYPE_AUDIO));
		qDebug() << aplay.Open();
		return true;
	}
	~Test()
	{
		isExist = true;
		video->isExit = true;
		vdecode.Close();
		adecode.Close();
		demux.Close();
		wait();
	}
protected:
	unsigned char* pcm = new unsigned char[1024 * 1024];
	void run()
	{
		for (;;)
		{
			if(isExist) break;
			AVPacket* pkt = demux.Read();

			if (pkt == NULL) {
				demux.Seek(0);
				qDebug() << "decode ok";
				//break;
			}
			if (demux.IsVideo(pkt))
			{
				vdecode.Send(pkt);
				for (;;)
				{
					AVFrame* frame = vdecode.Recv();
					if (!frame) break;
					//处理视频帧
					video->Repaint(frame);
					//av_frame_free(&frame);
				}
			}
			else
			{
				adecode.Send(pkt);
				for (;;)
				{
					AVFrame* frame = adecode.Recv();
					if (!frame) break;
					//处理音频帧
					int len = resample.AudioResample(frame, pcm);
					while (len > 0)
					{
						if (aplay.GetFree() >= len)
						{
							aplay.Write((char*)pcm, len);
							break;
						}
						QThread::msleep(1);
					}
				}
			}
		}
	}
	
};

class DrawYUV : public QThread
{

public:
	VideoCanvas* video;
	unsigned char *yuv[3] = { 0 };
	FILE *fp = NULL;
	bool isExist = false;

	void Init()
	{
		video->Init(768, 432);
		fp = fopen("F:/Http/Faded_yuv420p_768x432.yuv", "rb");
		if (!fp)
		{
			qDebug() << "open file failed";
		}
		yuv[0] = new unsigned char[768 * 432];
		yuv[1] = new unsigned char[768 * 432 / 4];
		yuv[2] = new unsigned char[768 * 432 / 4];
	}
	~DrawYUV()
	{
		isExist = true;
		video->isExit = true;
		wait();
	}
protected:
	void run()
	{
		for (;;)
		{
			if(isExist) break;
			if (feof(fp) != 0)
			{
				fseek(fp, 0, SEEK_SET);
				qDebug() << "read end";
			}

			fread(yuv[0], 1, 768 * 432, fp);
			fread(yuv[1], 1, 768 * 432 / 4, fp);
			fread(yuv[2], 1, 768 * 432 / 4, fp);

			video->Repaint(yuv);

		}
	}
};
int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	QtFFmpegPlayer w;
	w.show();

	Test test;
	test.video = w.ui.video;
	bool isOpenSuccess = false;
	if (argc > 1)
		isOpenSuccess = test.Init(argv[1]);
	else
		isOpenSuccess = test.Init();
	if (isOpenSuccess)
		test.start();

	/*DrawYUV draw;
	draw.video = w.ui.video;
	draw.Init();
	draw.start();*/
	
	return a.exec();
}
