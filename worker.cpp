#include "worker.h"
#include <QtScript/QtScript>
#include "3rd/json/json/json.h"
#include <QJsonParseError>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
#include<unistd.h>

//Worker* Worker::m_pInstance = NULL;
Worker::Worker(QObject *parent) : QObject(parent)
{

}
//login
void Worker::doLogin(UserInfo &userinfo)
{
        emit loginReady(Login(userinfo));
}

bool Worker::Login(UserInfo  &userInfo)
{
    QString cmd = "/usr/bin/getLoginStatus.py "+userInfo.uname+" "+userInfo.pwd+" "+serverIp + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.first()=="login success")
    {
        userInfo.uid = list.last();
        return true;
    }else
    {
        userInfo.uid = "";
        return false;
    }
}


//get vms
bool Worker::getVMs(QVector<VM_CONFIG> &vmArr,QString &vms, QString &err)
{
    QString cmd = "/usr/bin/getVMs.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIp + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        err = tr("Login failed: printed info nums less than 2");
        qDebug()<<err;
        return false;
    }
    if(list.contains("login failed"))
    {
        err = tr("login failed");
        return false;
    }else if(list.contains("login success"))
    {
        QByteArray cstr = list.last().toLatin1();

        if(!parseVMs(vmArr, cstr))
        {
            if(list.last()==list[1])
            {
                err="该用户下无虚拟机";
            }else
                err="解析机器列表失败";
            return false;
        }
            //QMessageBox::information(this, "警告", "解析虚拟机列表信息失败，请点击刷新，重新获取");
        vms = '\''+list.last()+'\'';

        return true;
    }
    return true;
}

bool Worker::parseVMs(QVector<VM_CONFIG> &vmArray,QByteArray &ba)
{
    vmArray.clear();
    QJsonParseError jsonpe;
    QJsonDocument json = QJsonDocument::fromJson(ba, &jsonpe);

    if(jsonpe.error == QJsonParseError::NoError)
    {
        if(json.isArray())
        {
            QJsonArray vmsobjs = json.array();
            for(int i=0; i<vmsobjs.size(); i++)
            {
                QJsonObject obj = vmsobjs[i].toObject();
                VM_CONFIG vm;
                vm.name = obj["name"].toString();
                vm.vid = obj["uuid"].toString();
                vmArray.append(vm);
            }
        }
        return true;
    }else
        return false;
}

bool Worker::getVMsIpPort(QVector<VM_CONFIG> &vmArr)
{
    QString cmd = "/usr/bin/soc.py "+serverIp + " "+QString::number(59000) +" 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
//    if(list.size()<5)
//    {
//        qDebug()<<tr("telnet: info nums less than 5");
//        return false;
//    }
    for(int i=0; i<list.size();i++)
    {
        if(list[i].contains("\"instances\"")&&parseVMsIpPort(vmArr, list[i]))
            return true;
    }
    return false;
}

bool Worker::parseVMsIpPort(QVector<VM_CONFIG> &vmArray,QString output)
{
    QScriptEngine engine;
    QScriptValue sc = engine.evaluate("value=" + output);

    if( sc.isArray())
    {
        QScriptValueIterator it(sc);
        while(it.hasNext())
        {
            it.next();

            QScriptValueIterator i(it.value().property("instances"));
            while(i.hasNext())
            {
                i.next();
                QString uuid = i.value().property("uuid").toString();
                for(int j=0; j<vmArray.size();j++)
                {
                    if(vmArray[j].vid == uuid)
                    {
                        vmArray[j].ip = i.value().property("hostip").toString();
                        vmArray[j].port = i.value().property("spiceport").toInt32();
                        if(vmArray[j].port == -1)
                        {
                            vmArray[j].status = SHUTDOWN;
                        }else
                            vmArray[j].status = RUNING;
                        break;
                    }
                }

            }
        }
        return true;
    }else
        return false;
}

bool Worker::getVMsInfo(QVector<VM_CONFIG> &vmArr,QString &vms)
{
    QString cmd = "/usr/bin/getVMsInfo.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIp + " " +vms+" 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.contains("login failed"))
    {
        //QMessageBox::information(this, "警告", "获取虚拟机详细信息失败，请点击刷新，重新获取");
        return false;
    }else if(list.contains("login success"))
    {
        QByteArray cstr = list.last().toLatin1();
        if(!parseVMsInfo(vmArr, cstr))
            return false;
            //QMessageBox::information(this, "警告", "解析虚拟机详细信息失败，请点击刷新，重新获取");
        return true;
    }
    return true;
}

bool Worker::parseVMsInfo(QVector<VM_CONFIG> &vmArray,QByteArray &ba)
{
    QJsonParseError jsonpe;
    QJsonDocument json = QJsonDocument::fromJson(ba, &jsonpe);
    if(jsonpe.error == QJsonParseError::NoError)
    {
        if(json.isArray())
        {
            QJsonArray vmsobjs = json.array();
            for(int i=0; i<vmsobjs.size(); i++)
            {
                QJsonObject obj = vmsobjs[i].toObject();
                QString id = obj["id"].toString();
                for(int j=0; j<vmArray.size();j++)
                {
                    if(id == vmArray[j].vid)
                    {
                        QJsonObject obj1 = obj["addresses"].toObject();
                        QJsonArray array = obj1["demo"].toArray();
                        for(int k=0; k<array.size();k++)
                        {
                            Addr addr;
                            addr.mac = (array[k].toObject())["OS-EXT-IPS-MAC:mac_addr"].toString();
                            addr.ip = (array[k].toObject())["addr"].toString();
                            addr.type = (array[k].toObject())["OS-EXT-IPS:type"].toString();
                            vmArray[j].addrs.append(addr);
                        }
                        vmArray[j].created =obj["created"].toString();
                        break;
                    }
                }

            }
        }
        return true;
    }else
        return false;
}


void Worker::doGetAllInfo(QVector<VM_CONFIG> *vmArr, QString &vms)
{
//    if(getVMs(*vmArr, vms)&&getVMsIpPort(*vmArr)&&getVMsInfo(*vmArr, vms))
//    {
//        emit getAllInfoReady(true);
//    }else
//        emit getAllInfoReady(false);
    qDebug()<<"doGetAllInfo beg";

    QString err = "";
    if(getVMs(*vmArr, vms, err))
    {
        qDebug()<<"getVMs success";
    }else
    {
        qDebug()<<"getVMs failed";
        emit getAllInfoReady(false, "getVMs error:"+err);
        return;
    }
    if(getVMsIpPort(*vmArr))
    {
        qDebug()<<"getVMsIpPort success";
    }else
    {
        qDebug()<<"getVMsIpPort failed";
        emit getAllInfoReady(false, "getVMsIpPort failed");
        return;
    }
    if(getVMsInfo(*vmArr, vms))
    {
        qDebug()<<"getVMsInfo success";
    }else
    {
        qDebug()<<"getVMsInfo failed";
        emit getAllInfoReady(false, "getVMsInfo failed");
        return;
    }
    emit getAllInfoReady(true, "get all info success");
//    qDebug()<<"getVMs(vmArr, vms):"<<getVMs(*vmArr, vms);
//    qDebug()<<"getVMsIpPort(vmArr):"<<getVMsIpPort(*vmArr);
//    qDebug()<<"getVMsInfo(vmArr, vms):"<<getVMsInfo(*vmArr, vms);
//    emit getAllInfoReady(true);

}

void Worker::doOperateVM(QVector<VM_CONFIG> *vmArr,QString vid, STAT*status)
{
    if(operateVMs(vid,status))
    {
        sleep(3);
        getVMsIpPort(*vmArr);
        emit operateVMReady(true, "");
    }else
    {
        emit operateVMReady(false, "开关及操作失败");
    }
}

bool Worker::operateVMs(QString vid, STAT *status)
{
    QString operate = *status==RUNING?"shutdown":"startup";
    QString cmd = "/usr/bin/vmOperate.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIp + " " +vid+" "+operate+" 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.contains("login failed"))
    {
        //QMessageBox::information(this, "警告", "获取虚拟机详细信息失败，请点击刷新，重新获取");
        return false;
    }else if(list.contains("login success"))
    {
        if(list.size()>2)
        {

            return false;
        }
        *status = *status ==RUNING? SHUTDOWN:RUNING;
        return true;
    }
    return true;
}

bool Worker::needUpdate()
{
    QString cmd = "/usr/bin/update.sh  2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    if(res == "0")
    {
        return false;
    }else
        return true;
}

bool Worker::update(QString &err)
{
    QString cmd = "pkexec -u root yum install cdos-desktop-cloud -y 2>&1;echo $?";
    //QString cmd = "pkexec -u root apt-get --reinstall install cdos-desktop-cloud 2>&1; echo $?";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList strlist = res.split('\n');
    if(strlist.length()<2)
    {
        err = tr("命令运行结果解析失败");
        return false;
    }
    if(strlist.last()!="0")
    {
        err = strlist[strlist.length()-2];
        return false;
    }
    return true;
}
