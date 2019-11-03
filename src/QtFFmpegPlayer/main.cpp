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
#pragma  comment(lib, "avdevice.lib")

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
class PlayPCM
{
public:
	
	static void Play1(const char* url, int sampleRate, int channels)
	{
		AudioPlay ap;
		ap.Open(sampleRate, channels);
		int size = ap.GetPeriodSize();
		char *buf = new char[size];           //创建缓冲区buf
		FILE *fp = fopen(url, "rb"); //打开音频文件audio1.pcm获取文件指针fp。r是以只读方式打开资源,b是不转义数据,就是不认转义字符,告诉函数库打开的文件为二进制文件，而非纯文字文件。注意如果写成FILE *fp = fopen("audio1.pcm", "r")会播放不了音频文件
		while (!feof(fp)) //测试文件指针是否到了文件结束的位置。也就是判断音频文件audio1.pcm是否读完了
		{
			if (ap.GetFree() < size)
			{
				QThread::msleep(1); //这里如果不加延时，运行代码时，CPU占用率会极大。如果延时时间太长，比如1000ms，播放时声音会一卡一卡，这里选择延时1ms。
				continue;
			}
			int len = fread(buf, 1, size, fp); //将音频文件audio1.pcm的PCM数据读取到内存buf中

			if (len <= 0)  //如果读取到文件末尾或者读取不成功则通过break函数跳出while循环
			{
				break;
			}
			ap.Write(buf, len);
		}
		fclose(fp);          //关闭文件描述符fp
		ap.Close();
	}
};

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		qputenv("VIDEO_PATH", QByteArray(argv[1]));
	}

	
	PlayPCM pp;
	pp.Play1("D:/Test/faded.pcm", 44100, 2);

	QApplication a(argc, argv);
	QtFFmpegPlayer w;
	w.show();
	
	return a.exec();
}
