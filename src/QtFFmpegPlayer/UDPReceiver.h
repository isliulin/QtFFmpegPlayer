#pragma once

#include <QObject>
#include <QUdpSocket>

class UDPReceiver : public QObject
{
	Q_OBJECT

public:
	UDPReceiver(QObject *parent);
	~UDPReceiver();

public slots:

	void OnReceive();


private:
	QUdpSocket * uSocket;
};
