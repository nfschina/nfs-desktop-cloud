#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include"common.h"
#include <QDialog>
#include "waitdialog.h"
#include <QThread>
#include "worker.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(UserInfo &usrInfo, QWidget *parent = 0);
    ~LoginDialog();
    QString getServerIP(){
        return serverIP;
    }

    bool Login();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_loginPushButton_clicked();
//    void on_quitPushButton_clicked();

    void handleLoginRes(bool success);

    void on_closeButton_clicked();

    

signals:
    void operate(UserInfo &userinfo);

private:
    void moveToCenter();
    void waitDiaogAppear();
    void waitDialogAccept();

    Ui::LoginDialog *ui;
    QString serverIP;
    bool m_Drag;
    QPoint m_DragPosition;
    // user info
    UserInfo  &m_userInfo;

    WaitDialog *waitD;
    QThread workerThread;
    Worker *worker;
    void paintEvent(QPaintEvent *event);

};

#endif // LOGINDIALOG_H
