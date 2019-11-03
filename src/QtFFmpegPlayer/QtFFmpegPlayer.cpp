#include "QtFFmpegPlayer.h"
#include "Player_1_0.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <PlayerUtility.h>
#include "UDPReceiver.h"
QtFFmpegPlayer::QtFFmpegPlayer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	player = new Player_1_0();
}

QtFFmpegPlayer::~QtFFmpegPlayer()
{
	delete player;
}

void QtFFmpegPlayer::showEvent(QShowEvent *event)
{
	
	static bool isOpenVideo = false;
	if (isOpenVideo) return;
	isOpenVideo = true;
	
	bool isOpenSuccess = false;

	QByteArray path = qgetenv("VIDEO_PATH");
	if (path.isEmpty())
	{
		isOpenSuccess = player->Open(ui.video, "D:/HTTPServer/Faded.mp4");
		//isOpenSuccess = player->Open(ui.video, "https://www.sttplay.com/assets/Faded.mp4");
	}
	else
	{
		qDebug() << path.constData();
		isOpenSuccess = player->Open(ui.video, path.constData());
	}

	if (!isOpenSuccess) return;

	int width = 0, height = 0;
	player->GetVideoSize(&width, &height);
	//屏幕分辨率
	int scrWidth, scrHeight;
	GetScreenSize(&scrWidth, &scrHeight);
	//缩小范围
	scrWidth *= 0.6f;
	scrHeight *= 0.6f;
	float rate = height * 1.0f / width;
	if (width > height)
	{
		//视频的宽度大于高度
		if (width > scrWidth) width = scrWidth;
		height = width * rate;
	}
	else
	{
		//视频的宽度小于高度
		if (height > scrHeight) height = scrHeight;
		width = height / rate;
	}
	resize(width, height);
	player->start();
}

void QtFFmpegPlayer::resizeEvent(QResizeEvent *event)
{
	ui.video->resize(this->size());
}

void QtFFmpegPlayer::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (isFullScreen())
		this->showNormal();
	else
		this->showFullScreen();
}

void QtFFmpegPlayer::keyPressEvent(QKeyEvent *ev)
{
	if (ev->key() == Qt::Key_Space)
	{
		PlayerUtility::Get()->isPause = !PlayerUtility::Get()->isPause;
	}
	if (ev->key() == Qt::Key_P)
	{
		PlayerUtility::Get()->isRunAudioTestThread = !PlayerUtility::Get()->isRunAudioTestThread;
	}
	QWidget::keyPressEvent(ev);
}

void QtFFmpegPlayer::GetScreenSize(int *width, int *height)
{

	QDesktopWidget *desktop = QApplication::desktop();
	int screenNum = desktop->screenCount();
	for (int i = 0; i < screenNum; i++)
	{
		QRect screen = desktop->screenGeometry();
		//qDebug("screen %d, width %d, height %d", i, screen.width(), screen.height());
		*width = screen.width();
		*height = screen.height();
	}

}
