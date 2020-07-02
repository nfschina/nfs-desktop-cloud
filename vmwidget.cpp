#include "vmwidget.h"
#include "ui_vmwidget.h"
#include "detaildialog.h"
#include "homewindow.h"
#include<unistd.h>
VMWidget::VMWidget(VM_CONFIG &vm,QWidget *parent) :
    QWidget(parent),
    m_vm(vm),
    prt(parent),
    ui(new Ui::VMWidget)
{
    ui->setupUi(this);
    this->setWindowOpacity(1);
    setStyleSheetByStatus();
    ui->label_vmName->setText(m_vm.name);
  //  ui->VMButton->setContextMenuPolicy(Qt::CustomContextMenu);
//    m_menu = new QMenu(this);
//    operAction = new QAction(this);
//    detailAction = new QAction(this);
//    detailAction->setText(tr("详细信息"));
    ui->VMLabel->hide();
    if(m_vm.status == RUNING)
    {
       // operAction->setText(tr("关机"));
        ui->run_pushButton->setStyleSheet("QPushButton { border-image: url(:/new/index/image/down.png);}\
                                            QPushButton:hover { border-image: url(:/new/index/image/down_hover.png);}");
    }else
    {
       // operAction->setText(tr("开机"));
        ui->run_pushButton->setStyleSheet("QPushButton { border-image: url(:/new/index/image/run.png);}\
                                        QPushButton:hover { border-image: url(:/new/index/image/run_hover.png);}");

    }
//    connect(operAction, SIGNAL(triggered()), this, SLOT(operateActionSlot()));
//    connect(detailAction, SIGNAL(triggered()), this, SLOT(detailActionSlot()));

//    m_menu->addAction(operAction);
//    m_menu->addAction(detailAction);

//    connect(ui->VMButton, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

    worker = new Worker;
    worker->moveToThread(&thread);
    connect(this, &VMWidget::operatVM, worker, &Worker::doOperateVM);
    connect(worker, &Worker::operateVMReady, this, &VMWidget::handleOperatVMRes);
    thread.start();

}

void VMWidget::operateActionSlot()
{
    //startup or shutdown
//    if(worker->operateVMs(m_vm.vid,&m_vm.status))
//        QMessageBox::information(NULL, "提示","开关机操作成功");
    HomeWindow *hw = static_cast<HomeWindow *>(prt);
    emit operatVM(&hw->vmArray, m_vm.vid, &m_vm.status);
    waitDiaogAppear();
//    worker->getVMsIpPort(hw->vmArray);
//    hw->updatetableUI();
 //   hw->emitOperateVM(&hw->vmArray, m_vm.vid, &m_vm.status);
  //  setStyleSheetByStatus();
}

void VMWidget::waitDialogAccept()
{
//    waitD->accept();
//    delete waitD;
    ui->VMLabel->hide();
    ui->VMButton->show();
}

void VMWidget::waitDiaogAppear()
{
//    waitD = new WaitDialog(this);
//    waitD->exec();
  //  waitD->deleteLater();
   QMovie *movie = new QMovie(":/new/index/image/loading.gif");
    movie->setScaledSize(QSize(243,176));
    ui->VMButton->hide();
    ui->VMLabel->show();
    ui->VMLabel->setMovie(movie);
    movie->start();

}


void VMWidget::handleOperatVMRes(bool success, QString err)
{
    waitDialogAccept();
    if(success)
    {
        HomeWindow *hw = static_cast<HomeWindow *>(prt);
        hw->updatetableUI();
        setStyleSheetByStatus();
        if(m_vm.status == RUNING)
        {
        ui->run_pushButton->setStyleSheet("QPushButton { border-image: url(:/new/index/image/down.png);}\
                                            QPushButton:hover { border-image: url(:/new/index/image/down_hover.png);}");
        //operAction->setText(tr("关机"));
        }
        else
        {
           // operAction->setText(tr("开机"));
            ui->run_pushButton->setStyleSheet("QPushButton { border-image: url(:/new/index/image/run.png);}\
                                                QPushButton:hover { border-image: url(:/new/index/image/run_hover.png);}");

        }
        QMessageBox::information(NULL, "提示","开关机操作成功");

    }else
    {
        QMessageBox::information(NULL, tr("警告"), QString("开关机操作失败：")+err);
    }
}

void VMWidget::detailActionSlot()
{
    //detail info
    DetailDialog dlg(m_vm);
    dlg.exec();
}

void VMWidget::showMenu(const QPoint &point)
{
    if(m_vm.status == RUNING)
    {
        operAction->setText(tr("关机"));
    }else
        operAction->setText(tr("开机"));
    m_menu->exec(mapToGlobal(point));
}

void VMWidget::setStyleSheetByStatus()
{
    if(m_vm.status == RUNING)
    {
        ui->VMButton->setStyleSheet(" QPushButton {  \
                          border: none; \
                          border-radius: 0px;   \
                          color:white;  \
                          font-weight:bold; \
                          image: url(:/new/index/run_namal);   \
                      } \
                      QPushButton:hover { \
                          border: 1px solid white; \
                          border-radius: 0px; \
                          color:white; \
                          font-weight:normal; \
                      } \
                      QPushButton:pressed { \
                          border: 1px solid white; \
                          border-radius: 0px; \
                          color:white; \
                          font-weight:bold; \
                      }");
    }else
    {
        ui->VMButton->setStyleSheet(" QPushButton {  \
                  border: none; \
                  border-radius: 0px;   \
                  color:white;  \
                  font-weight:bold; \
                  image: url(:/new/index/down_namal);   \
              } \
              QPushButton:hover { \
                  border: 1px solid white; \
                  border-radius: 0px; \
                  color:white; \
                  font-weight:normal; \
              } \
              QPushButton:pressed { \
                  border: 1px solid white; \
                  border-radius: 0px; \
                  color:white; \
                  font-weight:bold; \
              }");
    }

}

VMWidget::~VMWidget()
{
    delete ui;
    thread.quit();
    thread.wait();
}

void VMWidget::on_VMButton_clicked()
{
    emit emitData(m_vm);
}
void VMWidget::paintEvent(QPaintEvent *event)
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
       QColor color(124, 124, 124, 50);
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


void VMWidget::on_run_pushButton_clicked()
{
    operateActionSlot();
}

void VMWidget::on_detail_pushButton_clicked()
{
    detailActionSlot();
}
