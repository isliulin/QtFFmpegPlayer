#pragma once
#include <QThread>
class ProcessAudio : public QThread
{
public:
	ProcessAudio();
	~ProcessAudio();

protected:
	void run();
};

