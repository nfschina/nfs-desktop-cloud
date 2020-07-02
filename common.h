#ifndef COMMON
#define COMMON
#include<QString>
#include<QVector>
#include<QStringList>
#include <QtDebug>
#include<QMessageBox>

struct  UserInfo
{
    QString uname;
    QString uid;
    QString pwd;
};
enum STAT
{
    SHUTDOWN=0,
    RUNING,
};

struct Addr
{
    QString ip;
    QString mac;
    QString type;
};

struct VM_CONFIG{
    QString name;
    QString vid;
    QString ip;
    int port;
    STAT status;
    QString created;
    QVector<Addr> addrs;
};

typedef QVector<VM_CONFIG>   VMVECTOR;//TE策略列表类型

QString GetCmdRes(QString cmd);

#endif // COMMON

