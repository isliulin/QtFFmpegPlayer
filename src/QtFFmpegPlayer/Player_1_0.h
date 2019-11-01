#pragma once
#include <QThread>

class Demux;
class VideoCanvas;
class ProcessAudio;
class ProcessVideo;
class Player_1_0 : public QThread
{
public:
	Player_1_0();
	~Player_1_0();

	bool Open(VideoCanvas * canvas, const char* path);

	void GetVideoSize(int *width, int *height);

protected:
	void run();

private:
	Demux *demux = NULL;
	ProcessAudio *audio = NULL;
	ProcessVideo *video = NULL;

	bool isExit = false;

};

