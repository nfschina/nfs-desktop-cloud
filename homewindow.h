#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QWidget>
#include <QDateTime>
#include "common.h"
#include "mytitlebar.h"
#include <QMainWindow>
#include "worker.h"
#include "waitdialog.h"
#include<QThread>
#include<QProcess>
#include <qmath.h>
#include "updatedialog.h"
#include <QTimer>
namespace Ui {
class HomeWindow;
}

class QButtonGroup;
class QTranslator;
class QSettings;
class QMenu;


class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = 0);
    ~HomeWindow();
    void initLanguage();

    void updateViewUI();
    void updatetableUI();

    void clearLayout(QLayout *layout);

    void emitOperateVM(QVector<VM_CONFIG> *vmArr, QString vid, STAT *status);
signals:
    void getAllInfo(QVector<VM_CONFIG> *vmArr, QString &vms);
    void operatVM(QVector<VM_CONFIG> *vmArr, QString vid, STAT *status);

public slots:
    void handleGetAllInfoRes(bool success, QString error);
    void handleOperatVMRes(bool success, QString err);

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

    void openVm(VM_CONFIG vm);
    void openVmOfTable(int row, int column);

    void on_freshButton_clicked();

    void on_logoutButton_clicked();

    void tabChanged(int );
    void on_tableWidget_customContextMenuRequested(QPoint pos);
    void detailActionSlot();
    void operateActionSlot();

    void on_updateButton_clicked();

    void on_helpButton_clicked();

    void subProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void on_view_pushButton_clicked();
    void on_list_pushButton_clicked();
private:
    void moveToCenter();

    void initTitleBar();

    void waitDiaogAppear();
    void waitDialogAccept();
    void change_tablewidget_color();
private:
    Ui::HomeWindow *ui;

protected:
    MyTitleBar* m_titleBar;
    void paintEvent(QPaintEvent *event);
private:

    QString serverIp;
    UserInfo m_userInfo;
    QString vms;

    WaitDialog *waitD;
    QThread workerThread;
    Worker *worker;


    //table context menu
    QMenu *m_menu;
    QAction *operAction;
    QAction *detailAction;
    QMovie *movie;
public:
    QVector<VM_CONFIG> vmArray;

    int Max;
    int num;
    QProcess *p;
    updateDialog *updatedialog;
};


#endif // HOMEWINDOW_H
