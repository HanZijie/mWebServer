#ifndef LISTENTHREAD_H
#define LISTENTHREAD_H
#include <QThread>
#include <QTcpServer>
#include <QByteArray>
class ListenThread:public QThread
{
    Q_OBJECT
public:
    ListenThread(int port, QString fileRoute,int maxThread,int timeout,QString address);
    void run();
    int port;
    int maxThread;
    int timeout;

    QString fileRoute;
    QString address;

public slots:
    void recvThreadMessage1(QString msg1,QString msg2);

signals:
    void sendThreadMessage1(QString msg1,QString msg2);
};

#endif // LISTENTHREAD_H
