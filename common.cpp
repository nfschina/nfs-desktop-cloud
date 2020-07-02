#include "common.h"

QString GetCmdRes(QString cmd)
{
    FILE *pf;
    char buff[1000]={};
    QByteArray res ;
    pf = popen(cmd.toStdString().c_str(), "r");
    //fread(buff,sizeof(buff),sizeof(buff),pf);
    while(!feof(pf))
    {
        fread(buff,sizeof(buff)-1,1,pf);
        res += QByteArray(buff);
        //qDebug()<<buff;
        memset(buff,0,sizeof(buff));
    }

    pclose(pf);
    //qDebug()<<res;
    QString resStr(res);
    return resStr;
}
