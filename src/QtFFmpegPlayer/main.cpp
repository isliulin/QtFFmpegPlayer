#include "QtFFmpegPlayer.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include "Demux.h"
extern "C"
{
#include <libavformat/avformat.h>
}
#pragma  comment(lib, "avformat.lib")
#pragma  comment(lib, "avcodec.lib")
#pragma  comment(lib, "avutil.lib")
int main(int argc, char *argv[])
{
	qDebug() << avformat_configuration();
	Demux demux;
	demux.Open("D:/HTTPServer/Faded.mp4");
	demux.Open("D:/HTTPServer/Timian.mp4");
	demux.Seek(0.98);
	for (;;)
	{
		AVPacket* pkt = demux.Read();
		if(pkt == NULL) break;
	}

	QApplication a(argc, argv);
	QtFFmpegPlayer w;
	w.show();
	return a.exec();
}
