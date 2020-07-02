#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include"common.h"
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
//    static Worker*  getInstance()
//   {
//       if(m_pInstance == NULL)
//       {
//           m_pInstance = new SecFunClass();
//       }
//       return m_pInstance;
//   }
    void setSvrIP(QString ip){serverIp = ip;}
    void setUserInfo(UserInfo usr){m_userInfo=usr;}

    bool operateVMs(QString vid, STAT *status);
    bool getVMsIpPort(QVector<VM_CONFIG> &vmArr);

    bool needUpdate();
    bool update(QString &err);
private:
    bool Login(UserInfo  &userInfo);

    bool getVMs(QVector<VM_CONFIG> &vmArr, QString &vms, QString &err);
    bool parseVMs(QVector<VM_CONFIG> &vmArr,QByteArray &ba);
    bool parseVMsIpPort(QVector<VM_CONFIG> &vmArr,QString output);

    bool getVMsInfo(QVector<VM_CONFIG> &vmArr,QString &vms);
    bool parseVMsInfo(QVector<VM_CONFIG> &vmArr,QByteArray &ba);



signals:
    void loginReady(bool success);
    void getAllInfoReady(bool success, QString err);
    void operateVMReady(bool success,QString err);
public slots:
    void doLogin(UserInfo &userinfo);
    void doGetAllInfo(QVector<VM_CONFIG> *vmArr, QString &vms);
    void doOperateVM(QVector<VM_CONFIG> *vmArr,QString vid, STAT*status);
private:

    //static Worker *m_pInstance;
    QString serverIp;


    UserInfo m_userInfo;
};

#endif // WORKER_H
