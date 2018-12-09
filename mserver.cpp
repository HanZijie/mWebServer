#include "mserver.h"
#include <qdebug.h>
#include <QDataStream>
#include <iostream>
#include <fstream>
#include <QDateTime>
#include <QLocale>
#include <string>
#include <iostream>
#include <QDir>
using namespace std;
mServer::mServer(int port,QString fileRoute,int maxThread,int timeout,QString address)
{
    this->port = port;
    this->fileRoute = fileRoute;
    this->maxThread = maxThread;
    this->address = address;
    this->timeout = timeout;
    qDebug() << "thread pool set up";

}

void mServer::startServer(){
    if(!this->listen(QHostAddress(this->address),this->port)){
        qDebug() << tr("Server could not start because '%1' ").arg(this->errorString());
    }
    qDebug() << tr("Server started and is listening at %1:%2").arg(this->serverAddress().toString()).arg(this->serverPort());
}

void mServer::incomingConnection(qintptr socketDescriptor){
    maxThread-=1;
    if(maxThread > 0){
        qDebug()<<"Threadpool left:"<<maxThread;
        ParseRequest* runnable = new ParseRequest(socketDescriptor);
        runnable->fileRoute = this->fileRoute;
        runnable->timeout = this->timeout;
        connect(runnable,SIGNAL(sendSocketMessage1(QString,QString)),this,SLOT(recvSocketMessage2(QString,QString)));
        connect(runnable->socket,SIGNAL(disconnected()),this,SLOT(onFinished()));
        //runnable->start();
    }
}

void mServer::onFinished(){
    maxThread += 1;
    qDebug() << "Finished";
}

void mServer::recvSocketMessage2(QString msg1,QString msg2 ){
    //qDebug()<<"massage recved:" << msg1;
    emit sendSocketMessage2(msg1,msg2);
}


ParseRequest::ParseRequest(qintptr socketDescriptor){
    this->socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(socketDescriptor))
    {
        //emit error(socket->error());
        return;
    }
    connect(socket,SIGNAL(readyRead()),this,SLOT(onReadReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
    //socket->waitForReadyRead(this->timeout);
}

void ParseRequest::run(){
    QByteArray ba;
    QDataStream in(&ba,QIODevice::ReadWrite);
    in << socket->readAll();
    QByteArray request = ba;
    qDebug() << request;
    QByteArray fileName;
    int fileLeft = request.indexOf(' /');
    // 判断信息是否有效
    if(fileLeft == -1){
        return;
    }
    int fileRight = request.indexOf(" HTTP");

    // 获取文件名，得到文件路径和文件类型
    if(fileRight - fileLeft <= 1){
        fileName = "/index.html";
    }else{
        fileName = request.mid(fileLeft,fileRight-8);
    }

    // 获取文件
    qDebug() << QDir::currentPath();
    QString filePath = this->fileRoute.append(QString(fileName));
    int contentTypeNum = filePath.indexOf('.');
    QString contentType = filePath.right(filePath.length() - contentTypeNum - 1);
//    QFile* fp = new QFile("\index.html");
//    fp->open(QIODevice::ReadOnly);
//    if(!fp->exists()){
//        qDebug() << "file not exist";
//    }
    QByteArray* fileString;
    QString fileSize;
    string bufferStr;
    std::vector<char> buffer;
    long file_length;
    size_t file_size;
    //fp->close();
    if(contentType == "html"){
        qDebug() << "\nhtml:\n";
        FILE* fp = fopen("index.html","rb");
        if(fp == nullptr){
            qDebug() << "file not opne";
        }else{
            fseek(fp, 0, SEEK_END);
            file_length = ftell(fp);
            rewind(fp);

            buffer.resize(file_length);

            file_size = fread(&buffer[0], 1, file_length, fp);
            fileSize = QString::number((int)file_size);
            bufferStr.insert(bufferStr.begin(),buffer.begin(),buffer.end());
            fileString = new QByteArray(reinterpret_cast<const char*>(buffer.data()),buffer.size());
            qDebug() << file_length;
        }
        contentType = "text/html";
    }
    if(contentType == "png" || contentType.indexOf("jpg") != -1){
//        qDebug() << "png";
//        QFile* fp = new QFile("test.png");
//        //fp->open(QIODevice::ReadOnly);
//        if(!fp->exists()){
//            qDebug() << "file not exist";
//        }
//        fileString = QString::fromStdString(fp->readAll().toStdString());
//        fileSize = tr("%1").arg(fp->size());
//        fp->close();
        qDebug() << "\npng:\n";
        FILE* fp = fopen("test.jpg","rb");
        if(fp == nullptr){
            qDebug() << "file not opne";
        }else{
            fseek(fp, 0, SEEK_END);
            file_length = ftell(fp);
            rewind(fp);

            buffer.resize(file_length);

            file_size = fread(&buffer[0], 1, file_length, fp);
            fileSize = QString::number((int)file_size);
            qDebug() << fileSize << "\n\n";
            bufferStr.insert(bufferStr.begin(),buffer.begin(),buffer.end());
            fileString = new QByteArray(reinterpret_cast<const char*>(buffer.data()),buffer.size());
            qDebug() << file_length;
        }
        contentType = "image/jpeg";
    }
    // 获取日期
    QDateTime time = QDateTime::currentDateTime();
    QLocale eng = QLocale::English;
    QString strFormat = "ddd MMMM d yy, hh:mm:ss";
    QString timeString = eng.toString(time,strFormat);
    /*改成404 Not Found*/
    QString block_ = QString("HTTP/1.1 200 OK\r\nDate: Date: Sun, 11 Nov 2018 16:39:17 GMT\r\nServer: WSGIServer/0.2 CPython/3.6.1\r\n");
    block_ += QString("Content-Type: ") + contentType +QString("\r\nX-Frame-Options: SAMEORIGIN\r\nContent-Length: 292250\r\n\r\n");
    QByteArray block = block_.toUtf8() + *fileString;
    //QString block = QString("HTTP/1.1 200 OK\r\nDate: Sat, 31 Dec 2005 23:59:59 GMT\r\nContent-Type: text/html;charset=ISO-8859-1\r\nContent-Length: 122\r\n\r\n<html><head><title>Testpage</title></head><body>This is a test page.</body></html>");
    socket->write(block);
    socket->flush();
    /*添加一个发送失败的错误处理机制*/
    QString threadText = QStringLiteral("@0x%1").arg(quintptr(QThread::currentThreadId()), 16, 16, QLatin1Char('0'));
    QString msg1 = threadText + ":" + QString::fromStdString(request.toStdString()) ;
    //qDebug()<<"begin:" << QString::fromStdString(request.toStdString());
    QString msg2 = threadText + ":" + block + "\r\n\r\n";
    emit sendSocketMessage1(msg1,msg2);
}

void ParseRequest::onDisconnected(){
    socket->close();
    qDebug()<<"exit()";
    exit();
}

void ParseRequest::onReadReady(){
    start();
}


