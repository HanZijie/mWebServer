#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    QByteArray fileRoute;

private slots:
    void on_pushButton_OK_clicked();

    void on_lineEdit_route_returnPressed();

    void on_toolButton_clicked();

    void addBrowerText(QString msg1,QString msg2);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
