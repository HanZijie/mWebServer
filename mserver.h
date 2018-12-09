#ifndef MSERVER_H
#define MSERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>


class mServer:public QTcpServer
{
    Q_OBJECT

public:
    mServer(int port, QString fileRoute, int maxThread,int timeout,QString address);
    void startServer();
    int port;
    int maxThread;
    int timeout;
    QString fileRoute;
    QString address;
    void incomingConnection(qintptr socketDescriptor) override;

public slots:
    void onFinished();
    void recvSocketMessage2(QString msg1,QString msg2);
signals:
    void sendSocketMessage2(QString msg1,QString msg2);
};

class ParseRequest:public QThread
{
    Q_OBJECT
public:
    QTcpSocket* socket;
    ParseRequest(qintptr socketDescriptor);
    void run();
    QString fileRoute;
    int timeout;

public slots:
    void onReadReady();
    void onDisconnected();
signals:
    void sendSocketMessage1(QString msg1,QString msg2);

};
#endif // MSERVER_H
