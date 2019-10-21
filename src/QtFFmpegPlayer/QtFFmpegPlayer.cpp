#include "QtFFmpegPlayer.h"

QtFFmpegPlayer::QtFFmpegPlayer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
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
