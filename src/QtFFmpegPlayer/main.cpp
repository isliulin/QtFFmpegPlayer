#include "QtFFmpegPlayer.h"
#include <QtWidgets/QApplication>
#include <QDebug>
extern "C"
{
#include <libavformat/avformat.h>
}
#pragma  comment(lib, "avformat.lib")
int main(int argc, char *argv[])
{
	qDebug() << avformat_configuration();
	QApplication a(argc, argv);
	QtFFmpegPlayer w;
	w.show();
	return a.exec();
}
