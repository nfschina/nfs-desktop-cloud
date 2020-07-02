#include <QDesktopWidget>
#include<QMessageBox>
#include <qdebug.h>
#include <QEvent>
#include <QMouseEvent>
#include <QShortcut>
#include <QMetaType>
#include "logindialog.h"
#include "ui_logindialog.h"
#include<QSettings>
#include <QDir>
#include <QMutex>
#include <qmath.h>


LoginDialog::LoginDialog(UserInfo &usrInfo, QWidget *parent) :
    QDialog(parent),
    m_userInfo(usrInfo),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    moveToCenter();

    QRegExp ipRegExp = QRegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator *ipRegExpValidator = new QRegExpValidator(ipRegExp, this);

    ui->serverLineEdit->setValidator(ipRegExpValidator);
    ui->serverLineEdit->setPlaceholderText(tr("  server address"));
    ui->usernameLineEdit->setPlaceholderText(tr("  user name"));
    ui->passwdLineEdit->setPlaceholderText(tr("  user password"));
    ui->serverLineEdit->setFocus();
    //ui->loginPushButton->setShortcut(Qt::Key_Enter);

    QShortcut *key=new QShortcut(QKeySequence(Qt::Key_Enter), this);// 创建一个快捷键"Key_Return"键
    connect(key, SIGNAL(activated()), this, SLOT(on_loginPushButton_clicked()));//连接到指定槽函数

    //read settings
    ui->serverLineEdit->setText("192.168.162.120");
    ui->usernameLineEdit->setText("ztb20");
    ui->passwdLineEdit->setText("nfschina123");
    QSettings settings(QDir::homePath()+"/.cdos-desktop-cloud/config.conf", QSettings::IniFormat);
    ui->checkBox->setChecked(settings.value("LOGIN/rempwd").toInt());
    if(ui->checkBox->isChecked())
    {
        ui->serverLineEdit->setText(settings.value("LOGIN/ip").toString());
        ui->usernameLineEdit->setText(settings.value("LOGIN/uname").toString());
        QString cmd = "/usr/bin/deAesBase64.py " + settings.value("LOGIN/password").toString() + " 2>&1";
        QString res = GetCmdRes(cmd).trimmed();
        ui->passwdLineEdit->setText(res);

    }


    //thread
    worker = new Worker;
    worker->moveToThread(&workerThread);
    qRegisterMetaType<UserInfo> ("UserInfo &");
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &LoginDialog::operate, worker, &Worker::doLogin);
    connect(worker, &Worker::loginReady, this, &LoginDialog::handleLoginRes);
    workerThread.start();
}

bool LoginDialog::Login()
{
    QString cmd = "/usr/bin/getLoginStatus.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIP + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.first()=="login success")
    {
        m_userInfo.uid = list.last();
        return true;
    }else
    {
        m_userInfo.uid = "";
        return false;
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

/*鼠标按下事件*/
void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

/*鼠标移动事件*/
void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Drag && (event->buttons() && Qt::LeftButton)) {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

/*鼠标释放事件*/
void LoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_Drag = false;
}

void LoginDialog::on_loginPushButton_clicked()
{
    int pos = 0;
    this->serverIP = ui->serverLineEdit->text();
    m_userInfo.uname = ui->usernameLineEdit->text();
    QString userpassword = ui->passwdLineEdit->text();
    QString cmd = "/usr/bin/aesbase64.py " + userpassword + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    m_userInfo.pwd = res;

    qDebug()<<"password is :" << m_userInfo.pwd << endl;


    if(this->serverIP.isEmpty())
    {
        ui->serverLineEdit->setFocus();
        ui->serverLineEdit->setCursorPosition(ui->serverLineEdit->text().length());
       // ui->serverLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    if(ui->serverLineEdit->validator()->validate(this->serverIP, pos) != QValidator::Acceptable)
    {
        ui->serverLineEdit->setFocus();
        ui->serverLineEdit->setCursorPosition(ui->serverLineEdit->text().length());
       // ui->serverLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

   // ui->serverLineEdit->setStyleSheet("background-color: white;");

    if (m_userInfo.uname.isEmpty())
    {
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->setCursorPosition(ui->usernameLineEdit->text().length());
      //  ui->usernameLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    //ui->usernameLineEdit->setStyleSheet("background-color: white;");

    if(m_userInfo.pwd.isEmpty())
    {
        ui->passwdLineEdit->setFocus();
        ui->passwdLineEdit->setCursorPosition(ui->passwdLineEdit->text().length());
       // ui->passwdLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

   // ui->passwdLineEdit->setStyleSheet("background-color: white;");
    worker->setSvrIP(serverIP);
    emit operate(m_userInfo);
    waitDiaogAppear();
//    if(Login())
//        accept();
//    else
//        QMessageBox::warning(this, "警告", "登录失败,请确认用户名和密码无误");
}

void LoginDialog::handleLoginRes(bool success)
{
    waitDialogAccept();
    if(success)
    {
        //write to conf
        QSettings settings(QDir::homePath()+"/.cdos-desktop-cloud/config.conf", QSettings::IniFormat);
        settings.setValue("LOGIN/ip", ui->serverLineEdit->text());
        settings.setValue("LOGIN/uname", ui->usernameLineEdit->text());
        settings.setValue("LOGIN/password", ui->passwdLineEdit->text());
        settings.setValue("LOGIN/rempwd", ui->checkBox->isChecked()?1:0);
        accept();
    }else
    {
        QMessageBox::warning(this, "警告", "登录失败,请确认用户名和密码无误");
    }
}

//void LoginDialog::on_quitPushButton_clicked()
//{
//    reject();
//}

void LoginDialog::moveToCenter()
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}

void LoginDialog::waitDialogAccept()
{
    waitD->accept();
    delete waitD;
}

void LoginDialog::waitDiaogAppear()
{
    waitD = new WaitDialog(this);
    waitD->exec();
  //  waitD->deleteLater();
}
void LoginDialog::paintEvent(QPaintEvent *event)
{
       QPainter painter(this);
       painter.eraseRect(0,0,this->width(),this->height());
       QPainterPath path1;
       path1.setFillRule(Qt::WindingFill);
       path1.addRect(4, 4, 254, 128);
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

void LoginDialog::on_closeButton_clicked()
{
    QApplication::exit();
}


