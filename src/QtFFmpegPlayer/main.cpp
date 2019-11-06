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
#include "DrawYUV.h"

extern "C"
{
#include <libavformat/avformat.h>
}
#pragma  comment(lib, "avformat.lib")
#pragma  comment(lib, "avcodec.lib")
#pragma  comment(lib, "avutil.lib")
#pragma  comment(lib, "swresample.lib")
#pragma  comment(lib, "avdevice.lib")


class PlayPCM
{
public:
	
	static void Play1(const char* url, int sampleRate, int channels)
	{
		AudioPlay ap;
		ap.Open(sampleRate, channels);
		int size = ap.GetPeriodSize();
		char *buf = new char[size];           //����������buf
		FILE *fp = fopen(url, "rb"); //����Ƶ�ļ�audio1.pcm��ȡ�ļ�ָ��fp��r����ֻ����ʽ����Դ,b�ǲ�ת������,���ǲ���ת���ַ�,���ߺ�����򿪵��ļ�Ϊ�������ļ������Ǵ������ļ���ע�����д��FILE *fp = fopen("audio1.pcm", "r")�Ქ�Ų�����Ƶ�ļ�
		while (!feof(fp)) //�����ļ�ָ���Ƿ����ļ�������λ�á�Ҳ�����ж���Ƶ�ļ�audio1.pcm�Ƿ������
		{
			if (ap.GetFree() < size)
			{
				QThread::msleep(1); //�������������ʱ�����д���ʱ��CPUռ���ʻἫ�������ʱʱ��̫��������1000ms������ʱ������һ��һ��������ѡ����ʱ1ms��
				continue;
			}
			int len = fread(buf, 1, size, fp); //����Ƶ�ļ�audio1.pcm��PCM���ݶ�ȡ���ڴ�buf��

			if (len <= 0)  //�����ȡ���ļ�ĩβ���߶�ȡ���ɹ���ͨ��break��������whileѭ��
			{
				break;
			}
			ap.Write(buf, len);
		}
		fclose(fp);          //�ر��ļ�������fp
		ap.Close();
	}
};

#pragma pack(4)
int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		qputenv("VIDEO_PATH", QByteArray(argv[1]));
	}
	//PlayPCM pp;
	//pp.Play1("D:/Test/faded.pcm", 44100, 2);

	QApplication a(argc, argv);
	QtFFmpegPlayer w;
	w.show();

	//DrawYUV *canvas = new DrawYUV(FragmentType::I420, 852, 480, "F:/HTTPServer/ExcuseMe_852x480.yuv");
	////DrawYUV *canvas = new DrawYUV(FragmentType::NV12, 852, 480, "F:/HTTPServer/ExcuseMe_852x480.nv12");
	////DrawYUV *canvas = new DrawYUV(FragmentType::NV21, 852, 480, "F:/HTTPServer/ExcuseMe_852x480.nv21");
	//canvas->show();
	//canvas->InitializeCanvas();
	//canvas->start();
	
	return a.exec();
}
