#include "QtFFmpegPlayer.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QThread>
#include "Demux.h"
#include "Decode.h"
#include "VideoCanvas.h"
#include "VideoCanvas2.h"

extern "C"
{
#include <libavformat/avformat.h>
}
#pragma  comment(lib, "avformat.lib")
#pragma  comment(lib, "avcodec.lib")
#pragma  comment(lib, "avutil.lib")

class Test : public QThread
{

public:
	Demux demux;
	Decode adecode;
	Decode vdecode;
	VideoCanvas* video;
	void Init()
	{
		qDebug() << avformat_configuration();
		
		demux.Open("F:/Http/Faded.mp4");
		//demux.Open("D:/HTTPServer/4K.mp4");
		//demux.Open("rtmp://192.168.0.103/live/test");
		//demux.Seek(0.98);
		
		vdecode.Open(demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO));
		vdecode.Close();
		vdecode.Open(demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO));
		adecode.Open(demux.GetMediaParameters(AVMEDIA_TYPE_AUDIO));
		video->Init(demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO)->width, demux.GetMediaParameters(AVMEDIA_TYPE_VIDEO)->height);
	}
	
protected:
	void run()
	{
		for (;;)
		{
			//QThread::msleep(35);
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
					video->Repaint(frame);
				}
			}
			else
			{
				adecode.Send(pkt);
				for (;;)
				{
					AVFrame* frame = adecode.Recv();
					if (!frame) break;
					av_frame_free(&frame);
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

protected:
	void run()
	{
		for (;;)
		{
			if (feof(fp) != 0)
			{
				fseek(fp, 0, SEEK_SET);
				qDebug() << "read end";
			}

			fread(yuv[0], 1, 768 * 432, fp);
			fread(yuv[1], 1, 768 * 432 / 4, fp);
			fread(yuv[2], 1, 768 * 432 / 4, fp);

			video->Repaint2(yuv);
		}
	}
};
int main(int argc, char *argv[])
{
	

	QApplication a(argc, argv);
	QtFFmpegPlayer w;
	w.show();

	/*Test test;
	test.video = w.ui.video;
	test.Init();
	test.start();*/

	

	//DrawYUV draw;
	/*draw.video = w.ui.video;
	draw.Init();
	draw.start();*/
	
	

	return a.exec();
}
