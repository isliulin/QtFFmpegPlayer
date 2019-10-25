#include "ProcessBase.h"
#include <QDebug>


ProcessBase::ProcessBase()
{
}


ProcessBase::~ProcessBase()
{
}

void ProcessBase::Push(AVPacket* pkt)
{
	if (!pkt) return;
	while (packets.size() > 100)
	{
		QThread::msleep(1);
	}
	QMutexLocker locker(&tmpMtx);
	tmpPkts.push_back(pkt);

}