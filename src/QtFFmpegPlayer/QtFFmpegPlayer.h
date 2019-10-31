#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtFFmpegPlayer.h"
class Player_1_0;
class QtFFmpegPlayer : public QWidget
{
	Q_OBJECT

public:
	QtFFmpegPlayer(QWidget *parent = Q_NULLPTR);
	~QtFFmpegPlayer();

public:
	Ui::QtFFmpegPlayerClass ui;

protected:

	void showEvent(QShowEvent *event);


	void resizeEvent(QResizeEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *event);

	void keyPressEvent(QKeyEvent *ev);

private:
	Player_1_0 *player = NULL;

	void GetScreenSize(int *width, int *height);
};
