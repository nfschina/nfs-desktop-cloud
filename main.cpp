#include "homewindow.h"
#include <QApplication>
#include <QFile>
//#include<QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTextCodec * codec = QTextCodec::codecForHtml("UTF-8");
//    QTextCodec::setCodecForLocale(codec);
    QFile *qss = new QFile(":/new/qss/style");
   // QFile *qss = new QFile("/home/nfs/program/cloud-client/style/style.qss");
    qss->open(QFile::ReadOnly);
    QString styleSheet = qss->readAll();
    a.setStyleSheet(styleSheet);
    qss->close();
    qss->deleteLater();
    HomeWindow w;
    w.show();

    return a.exec();
}
