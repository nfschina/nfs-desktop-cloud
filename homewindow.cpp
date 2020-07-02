#include "homewindow.h"
#include "ui_homewindow.h"
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QTranslator>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include "simple_log.h"
#include "vmwidget.h"
#include <QtScript/QtScript>
#include <QMenu>
#include <QEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include "logindialog.h"
#include "3rd/json/json/json.h"
#include <qdebug.h>
#include <QTcpSocket>
#include <QHostAddress>

#include <QJsonParseError>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
#include "detaildialog.h"
#include "helppage.h"

#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>

HomeWindow::HomeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    moveToCenter();

    //初始化语言

    LoginDialog dia(m_userInfo);
    if (QDialog::Accepted == dia.exec())
    {
        this->serverIp = dia.getServerIP();
        qDebug() << this->serverIp << endl;
       // ui->label->setText("<html><head/><body><p><span style=\" font-size:14pt; font-weight:600; color:#458b67;\">你好，"+m_userInfo.uname+"，欢迎使用方德云客户端！</span></p></body></html>");
    }
    else
    {
        exit(1);
    }

    //thread
    worker = new Worker;
    worker->moveToThread(&workerThread);
    qRegisterMetaType<VMVECTOR> ("QVector<VM_CONFIG> *");
    qRegisterMetaType<QString> ("QString &");
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &HomeWindow::getAllInfo, worker, &Worker::doGetAllInfo);
    connect(worker, &Worker::getAllInfoReady, this, &HomeWindow::handleGetAllInfoRes);
    connect(this, &HomeWindow::operatVM, worker, &Worker::doOperateVM);
    connect(worker, &Worker::operateVMReady, this, &HomeWindow::handleOperatVMRes);
    workerThread.start();

    //get vms
    worker->setSvrIP(serverIp);
    worker->setUserInfo(m_userInfo);
    emit getAllInfo(&vmArray,vms);
   // waitDiaogAppear();

    //initUI
    initTitleBar();
    ui->refresh_label->hide();
    ui->fresh_label->hide();
    ui->tabWidget->setCurrentIndex(1);
    ui->list_pushButton->setStyleSheet("QPushButton {\
                                       border-image: url(:/new/index/image/list_now.png);}\
                                       ");
    setWindowIcon(QIcon(":/new/index/taskbar"));
    this->setWindowFlags(Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);    // 设置尺寸属性
    setMouseTracking(true);    // 界面拉伸需要这个属性
    ui->tabWidget->tabBar()->hide();
    connect(ui->tabWidget, SIGNAL(currentChanged(int )), this, SLOT(tabChanged(int )));

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(0,50);
    ui->tableWidget->setColumnWidth(3,100);
    ui->tableWidget->setColumnWidth(5,100);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);

   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    QHeaderView* headerView = ui->tableWidget->verticalHeader();
   headerView->setHidden(true);
   ui->tableWidget->horizontalHeader()->setVisible(true);

     //设置表头内容
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(175,212,251);font:10pt '宋体' bold;color: white;border:none;};");
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->tableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );
    ui->tableWidget->horizontalHeader()->setFixedHeight(50);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &HomeWindow::openVmOfTable);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);   //右键菜单
    m_menu = new QMenu(ui->tableWidget);
    operAction = new QAction(QIcon(":/new/index/image/down2.png"),tr("shutdown"),this);
    detailAction = new QAction(QIcon(":/new/index/image/check2.png"),tr("details"),this);
    detailAction->setText(tr("详细信息"));

    operAction->setText(tr("关机"));
    m_menu->setStyleSheet("QMenu::item {color: #1283fb;}\
                            QMenu::item::selected {color: #6d49e2}");
//     operAction=new QWidgetAction(m_menu);
//     detailAction=new QWidgetAction(m_menu);
//     operButton=new QPushButton();
//     operButton->setStyleSheet("border-image:/new/index/image/run.png");
//     operAction->setDefaultWidget(operButton);
//     detailButton= new QPushButton();
//     detailButton->setStyleSheet("border-image:/new/index/image/check.png");
//     detailAction->setDefaultWidget(detailButton);
    m_menu->addAction(operAction);
    m_menu->addAction(detailAction);

    connect(operAction, SIGNAL(triggered()), this, SLOT(operateActionSlot()));
    connect(detailAction, SIGNAL(triggered()), this, SLOT(detailActionSlot()));


    //ui->logoutButton->setHidden(true);

    //read config
    QSettings settings(QDir::homePath()+"/.cdos-desktop-cloud/config.conf", QSettings::IniFormat);
    Max = settings.value("Mechine/maxnum").toInt()==0?3:settings.value("Mechine/maxnum").toInt();
    num = 0;
}

void HomeWindow::emitOperateVM(QVector<VM_CONFIG> *vmArr, QString vid, STAT *status)
{
    emit operatVM(vmArr, vid, status);
    waitDiaogAppear();
}

void HomeWindow::operateActionSlot()
{
    //startup or shutdown
    int row = ui->tableWidget->currentRow();
    if(row<0)
    {
        QMessageBox::information(NULL, "提示","请选择操作的虚拟机");
        return;
    }

    emitOperateVM(&vmArray, vmArray[row].vid, &vmArray[row].status);

//    if(worker->operateVMs(vmArray[row].vid,&vmArray[row].status))
//    {
//        sleep(3);
//        worker->getVMsIpPort(vmArray);
//        updatetableUI();
//        updateViewUI();

//        QMessageBox::information(NULL, "提示","开关机操作成功");
//    }
}

void HomeWindow::handleOperatVMRes(bool success, QString err)
{
    //waitDialogAccept();
    ui->refresh_label->hide();
    ui->fresh_label->hide();
    if(success)
    {
        updatetableUI();
        updateViewUI();
        QMessageBox::information(NULL, "提示","开关机操作成功");
    }else
    {
        QMessageBox::information(NULL, tr("警告"), QString("开关机操作失败：")+err);
    }
}

void HomeWindow::detailActionSlot()
{
    //detail info
    int row = ui->tableWidget->currentRow();
    if(row<0)
    {
        QMessageBox::information(NULL, "提示","请选择操作的虚拟机");
        return;
    }
    DetailDialog dlg(vmArray[row]);
    dlg.exec();
}



void HomeWindow::on_tableWidget_customContextMenuRequested(QPoint pos)
{
    m_menu->addAction(operAction);
    m_menu->addAction(detailAction);
    int row = ui->tableWidget->currentRow();
    if(row<0)
    {
        return;
    }
    if(vmArray[row].status == RUNING)
    {
        operAction->setText(tr("关机"));
    }else
        operAction->setText(tr("开机"));
    m_menu->exec(QCursor::pos());
    pos.isNull();
}

void HomeWindow::tabChanged(int index)
{
    if(index ==1)
        updatetableUI();
}

void HomeWindow::handleGetAllInfoRes(bool success, QString error)
{
    //waitDialogAccept();
    ui->refresh_label->hide();
    ui->fresh_label->hide();
    if(!success)
    {
        QMessageBox::warning(this, "警告", "获取虚拟机信息失败,错误信息：\n"+error);
    }
    QVector<VM_CONFIG> tmpArray;    //解决控制节点上的虚拟机也能显示的问题
    foreach (VM_CONFIG vm, vmArray) {
        if(!vm.ip.isEmpty() && !vm.created.isEmpty())
        {
            tmpArray.append(vm);
        }
    }
    vmArray = tmpArray;
    updateViewUI();
    updatetableUI();
}

void HomeWindow::clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0)) != 0){
        //删除widget
        if(item->widget()){
            delete item->widget();
            //item->widget()->deleteLater();
        }
        //删除子布局
        QLayout *childLayout = item->layout();
        if(childLayout){
            clearLayout(childLayout);
        }
        delete item;
    }
}

void HomeWindow::updatetableUI()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(vmArray.size());
    ui->tableWidget->setToolTip(tr("右键可对用户进行操作"));
    for(int i=0; i<vmArray.size(); i++)
    {
        QIcon runicon;
        runicon.addFile(":/new/index/image/run_small.png", QSize(32,32));
        QIcon downicon;
        downicon.addFile(":/new/index/image/down_small.png", QSize(32,32));
        if(vmArray[i].status==RUNING)
           // ui->tableWidget->setItem(i, 0, new QTableWidgetItem(runicon,vmArray[i].name));

            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(runicon,""));
        else
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(downicon,""));

          //  ui->tableWidget->setItem(i, 0, new QTableWidgetItem(downicon,vmArray[i].name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(vmArray[i].name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(vmArray[i].vid));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(vmArray[i].status)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(vmArray[i].ip));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(vmArray[i].port)));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(vmArray[i].created));
    }
    change_tablewidget_color();
}
void HomeWindow::change_tablewidget_color()
{
    for (int i = 0;i < ui->tableWidget->rowCount();i++)
    {
        if (i % 2 != 0)
        {
            for (int j = 0;j < ui->tableWidget->columnCount();j++)
            {
                QTableWidgetItem *item = ui->tableWidget->item(i,j);
                if (item)
                {
                const QColor color = QColor(224,237,251);
                item->setBackgroundColor(color);
                }
            }
        }
    }

}

void HomeWindow::updateViewUI()
{
    clearLayout(ui->vmsGridLayout);
    for(int i=0; i<vmArray.size(); i++)
   {
       VMWidget *vm = new VMWidget(vmArray[i],this);
        vm->setSvrIP(serverIp);
        vm->setUserInfo(m_userInfo);
       ui->vmsGridLayout->addWidget(vm, i/4, i%4);
       connect(vm, &VMWidget::emitData, this, &HomeWindow::openVm);
   }
}

void HomeWindow::openVmOfTable(int row, int column)
{
    if(row<0 || column<0)
    {
        QMessageBox::information(NULL, tr("提示"),tr("请选中要打开的虚拟机"));
        return;
    }

    openVm(vmArray[row]);
}

void HomeWindow::subProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    num--;
    qDebug()<<"num="<<num<<" exitcode="<<exitCode<<" exitStatus="<<exitStatus;
    sender()->deleteLater();
}
void HomeWindow::openVm(VM_CONFIG vm)
{
    if(vm.status!=RUNING)
    {
        QMessageBox::information(this, tr("提示"),tr("该虚拟主机不在线"));
        return;
    }
    //QString cmd = QString("%1 spice://%2:%3 --full-screen  &").arg("/usr/bin/remote-viewer").arg(vm.ip).arg(vm.port);
    QString cmd = QString("%1 spice://%2:%3 -t %4 ").arg("/usr/bin/remote-viewer").arg(vm.ip).arg(vm.port).arg(vm.name);
//    QString cmd = "/usr/bin/remote-viewer";
//    QString args = QString("spice://%1:%2 -t %3").arg(vm.ip).arg(vm.port).arg(vm.name);
//    GetCmdRes(cmd);

    if(num<Max)
    {
        p= new QProcess(this);
        connect(p, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &HomeWindow::subProcessFinished);
        p->start(cmd);
        if(p->waitForStarted())
        {
            num++;
        }
    }else
    {
        QMessageBox::information(this, tr("提示"),tr("允许的虚拟机超过最高数"));
    }
}

void HomeWindow::initTitleBar()
{
    m_titleBar = new MyTitleBar(this);
    m_titleBar->move(0, 0);
    //m_titleBar->setTitleIcon(":/new/index/mainwintitle",QSize(145, 30));

    QString title = "<html><head/><body><p><span style=\" font-size:9pt;  color:#999999;\">你好，</span><span style=\" font-size:9pt;  color:#999999;\">"+m_userInfo.uname+"</span><span style=\" font-size:9pt;  color:#999999;\">，欢迎使用方德桌面云客户端！</span></p></body></html>";
    //QString title = "nihao";
    m_titleBar->setTitleContent(title, 11);
    m_titleBar->setBackgroundColor(255, 255, 255);
    m_titleBar->setButtonType(MIN_MAX_BUTTON);
    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));

}

void HomeWindow::onButtonMinClicked()
{
    showMinimized();
}

void HomeWindow::onButtonRestoreClicked()
{
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
   // ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    //ui->tableWidget->setColumnWidth(2,80);
    //ui->tableWidget->setColumnWidth(4,80);
    QPoint windowPos;
    QSize windowSize;
    m_titleBar->getRestoreInfo(windowPos, windowSize);
    this->setGeometry(QRect(windowPos, windowSize));
}

void HomeWindow::onButtonMaxClicked()
{
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);


    m_titleBar->saveRestoreInfo(this->pos(), QSize(this->width(), this->height()));
    QRect desktopRect = QApplication::desktop()->availableGeometry();
    QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
    setGeometry(FactRect);
}

void HomeWindow::onButtonCloseClicked()
{
    close();
}

void HomeWindow::moveToCenter()
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}



HomeWindow::~HomeWindow()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}



//添加快捷键
void HomeWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::ControlModifier)
    {
            if(e->key() == Qt::Key_F10)
            {
                setWindowState(Qt::WindowFullScreen);
            }
            if(e->key() == Qt::Key_F11)
            {
                setWindowState(Qt::WindowMinimized);
            }

            if(e->key() == Qt::Key_F12)
            {
                setWindowState(Qt::WindowNoState);
            }
    }
    if(e ->modifiers() == Qt::Key_Meta)
    {
        return;
    }
    if(e ->modifiers() == Qt::AltModifier  && e ->key() == Qt::Key_Tab)
    {
        e->ignore();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}

void HomeWindow::waitDialogAccept()
{
    waitD->accept();
    delete waitD;
}

void HomeWindow::waitDiaogAppear()
{
    waitD = new WaitDialog(this);
    waitD->exec();
  //  waitD->deleteLater();
}


void HomeWindow::on_freshButton_clicked()
{
    emit getAllInfo(&vmArray,vms);
    movie = new QMovie(":/new/index/image/loading_fresh.gif");
    movie->setScaledSize(QSize(100,15));
    ui->refresh_label->setMovie(movie);
    movie->start();
    ui->fresh_label->show();
    ui->refresh_label->show();

   // waitDiaogAppear();
//    clearLayout(ui->vmsGridLayout);
}

void HomeWindow::on_logoutButton_clicked()
{
    exit(0);
}

void HomeWindow::on_updateButton_clicked()
{
    updatedialog=new updateDialog(this);
    updatedialog->update_start_timer();
    updatedialog->exec();

    if(worker->needUpdate())
    {
        if(QMessageBox::question(NULL, tr("提示"), tr("检测到新版本，是否需要更新？"))==QMessageBox::No)
        {
            return;
        }else
        {
            QString err = "";
            if(worker->update(err))
                QMessageBox::information(NULL, tr("提示"), tr("升级结束"));
            else
                QMessageBox::information(NULL, tr("提示"), tr("升级失败:")+err);
        }

    }else
    {
        QMessageBox::information(NULL, tr("提示"), tr("已是最新版本，不需要升级"));
    }

}

void HomeWindow::on_helpButton_clicked()
{
    HelpPage helpPage;
    helpPage.exec();
}
void HomeWindow::paintEvent(QPaintEvent *event)
{
       QPainter painter(this);
       painter.eraseRect(0,0,this->width(),this->height());
//       QPainterPath path1;
//       path1.setFillRule(Qt::WindingFill);
//       path1.addRect(4, 4, 254, 128);
       QPalette pal = palette();
       pal.setColor(QPalette::Background,QColor(0x00,0xff,0x00,0x00));
       setPalette(pal);
       painter.setRenderHint(QPainter::Antialiasing);
       QColor color(41, 36, 33, 50);
       for(int i=0; i<4; i++)
       {
           QPainterPath path;
           path.setFillRule(Qt::WindingFill);
           path.addRect(4-i, 4-i, this->width()-(4-i)*2, this->height()-(4-i)*2);
           color.setAlpha(90 - qSqrt(i)*40);
           painter.setPen(color);
           painter.drawPath(path);
       }
}

void HomeWindow::on_view_pushButton_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->view_pushButton->setStyleSheet("QPushButton {border-image: url(:/new/index/image/view_now.png);}");
    ui->list_pushButton->setStyleSheet("QPushButton {border-image: url(:/new/index/image/list_normal.png);}\
                                       QPushButton:hover {border-image: url(:/new/index/image/list_now.png);}");
}

void HomeWindow::on_list_pushButton_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->list_pushButton->setStyleSheet("QPushButton {border-image: url(:/new/index/image/list_now.png);}");
    ui->view_pushButton->setStyleSheet("QPushButton {border-image: url(:/new/index/image/view_normal.png);}\
                                       QPushButton:hover {border-image: url(:/new/index/image/view_now.png);}");

}
