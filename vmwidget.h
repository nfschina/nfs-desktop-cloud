#ifndef VMWIDGET_H
#define VMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "common.h"
#include<QAction>
#include <QMenu>
#include "worker.h"
#include <QThread>
#include "waitdialog.h"
#include <qmath.h>

namespace Ui {
class VMWidget;
}

class VMWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VMWidget(VM_CONFIG &vm, QWidget *parent = 0);
    ~VMWidget();
    void setStyleSheetByStatus();

    void setSvrIP(QString ip){worker->setSvrIP(ip);}
    void setUserInfo(UserInfo usr){worker->setUserInfo(usr);}
signals:
    void emitData(VM_CONFIG vm);

    void operatVM(QVector<VM_CONFIG> *vmArr, QString vid, STAT *status);


private slots:
    void on_VMButton_clicked();
    void showMenu(const QPoint &point);
    void operateActionSlot();
    void detailActionSlot();

    void handleOperatVMRes(bool success, QString err);

    void on_run_pushButton_clicked();

    void on_detail_pushButton_clicked();

private:
    void waitDiaogAppear();
    void waitDialogAccept();
private:
    Ui::VMWidget *ui;
    VM_CONFIG &m_vm;

    WaitDialog *waitD;

    //context menu
    QMenu *m_menu;
    QAction *operAction;
    QAction *detailAction;
    Worker *worker;

    QWidget *prt;
    QThread thread;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // VMWIDGET_H
