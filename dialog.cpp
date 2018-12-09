#include "dialog.h"
#include "ui_dialog.h"
#include "listenthread.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QObject>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_OK_clicked()
{
    int port = ui->spinBox_port->text().toInt();
    int maxThread = ui->lineEdit_maxThread->text().toInt();
    int timeout = ui->lineEdit_timeout->text().toInt();
    QString fileRoute = ui->lineEdit_route->text();
    QString address = ui->lineEdit_address->text();

    if(port <= 1024){
        QMessageBox* msg = new QMessageBox();
        msg->setText("端口号应该大于:1024");
        msg->show();
        return;
    }

    ListenThread* t1 = new ListenThread(port,fileRoute,maxThread,timeout,address);
    connect(t1,SIGNAL(sendThreadMessage1(QString,QString)),this,SLOT(addBrowerText(QString,QString)));
    t1->run( );

}

void Dialog::on_lineEdit_route_returnPressed()
{
    this->on_pushButton_OK_clicked();
}

void Dialog::on_toolButton_clicked()
{
    QString Route = QFileDialog::getExistingDirectory(NULL,"工作文件夹","/").toLocal8Bit();
    ui->lineEdit_route->setText(Route);
}

void Dialog::addBrowerText(QString msg1, QString msg2){
    //qDebug() <<"addBrower:\n" << msg1;
    ui->textBrowser_->append(msg2);
    ui->textBrowser_request->append(msg1);
    ui->textBrowser_->repaint();
    ui->textBrowser_request->repaint();
}
