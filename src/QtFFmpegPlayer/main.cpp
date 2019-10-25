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
#include "ProcessAudio.h"
#include "ProcessVideo.h"

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

	ProcessAudio pa;
	ProcessVideo pv;
	bool isExist = false;
	bool Init(VideoCanvas * canvas, const char* path = NULL)
	{
		
		qDebug() << avformat_configuration();

		bool isOpenSuccess = false;
		if (path) isOpenSuccess = demux.Open(path);
		else
		{
			isOpenSuccess = demux.Open("F:/HTTP/Faded.mp4");
			isOpenSuccess = demux.Open("F:/硕鼠下载/Dragon PigCNBALLERCloud Wang.mp4");
			isOpenSuccess = demux.Open("F:/Http/体面 于文文.mp4");
			//isOpenSuccess = demux.Open("C:/Users/Administrator/Desktop/dl/nfdw.mp4");
			//isOpenSuccess = demux.Open("C:/Users/Administrator/Desktop/dl/8K机房监控.mp4");
		}
		//demux.Seek(0.5);
		if (!isOpenSuccess)
		{
			qDebug() << "open failed!";
			return false;
		}
		
		pv.SetCanvas(canvas);
		if (!pv.Open(demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO))) return false;
		pv.start();
		if (!pa.Open(demux.GetMediaParameters(AVMEDIA_TYPE_AUDIO))) return false;
		pa.start();
		return true;
	}
	~Test()
	{
		isExist = true;
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
				pv.Push(pkt);
				//av_packet_free(&pkt);
			}
			else
			{
				pa.Push(pkt);
				//av_packet_free(&pkt);
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

	bool isOpenSuccess = false;
	if (argc > 1)
		isOpenSuccess = test.Init(w.ui.video, argv[1]);
	else
		isOpenSuccess = test.Init(w.ui.video);
	if (isOpenSuccess)
		test.start();

	/*DrawYUV draw;
	draw.video = w.ui.video;
	draw.Init();
	draw.start();*/
	
	return a.exec();
}
