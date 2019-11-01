#pragma once

#include <QObject>
#include <QUdpSocket>

class UDPReceiver : public QObject
{
	Q_OBJECT

public:
	UDPReceiver(QObject *parent = Q_NULLPTR);
	~UDPReceiver();

	void SendTo(QByteArray msg, QString host, int port);

public slots:

	void OnReceive();


private:
	QUdpSocket * uSocket;
};
