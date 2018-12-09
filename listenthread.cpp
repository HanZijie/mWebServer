#include "listenthread.h"
#include "mserver.h"
#include <qdebug.h>

ListenThread::ListenThread(int port, QString fileRoute,int maxThread,int timeout,QString address)
{
    this->fileRoute = fileRoute;
    this->port = port;
    this->maxThread = maxThread;
    this->timeout = timeout;
    this->address = address;
}

void ListenThread::run(){
    mServer* m_server;
    m_server = new mServer(this->port,this->fileRoute,this->maxThread,this->timeout,this->address);
    m_server->startServer();
    connect(m_server,SIGNAL(sendSocketMessage2(QString,QString)),this,SLOT(recvThreadMessage1(QString,QString)));
    this->exec();
}

void ListenThread::recvThreadMessage1(QString msg1,QString msg2)
{
    //qDebug() << "ListenThread:" <<msg1;
    emit sendThreadMessage1(msg1,msg2);
}



