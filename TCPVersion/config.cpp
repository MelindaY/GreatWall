#include "config.h"
#include <QFile>
#include <QTextStream>

Config::Config()
{

}

void Config::loadConfig(const QString &filename)
{
    QFile configfile(filename);
    configfile.open(QFile::ReadOnly);
    QTextStream stream(&configfile);
    QString _port;
    QString _ip;
    QString _mysql_user;
    QString _mysql_psw;
    QString _mysql_dbname;
    QString _ipBackup;
    int i,j;
    //��ȡport
    _port=stream.readLine();
    i=_port.indexOf(":");
    j=_port.indexOf(";");
    port=_port.mid(i+1,j-i-1);

    //��ȡip
    _ip=stream.readLine();
    i=_ip.indexOf(":");
    j=_ip.indexOf(";");
    ip=_ip.mid(i+1,j-i-1);

    //��ȡ�û�
    _mysql_user=stream.readLine();
    i=_mysql_user.indexOf(":");
    j=_mysql_user.indexOf(";");
    mysql_user=_mysql_user.mid(i+1,j-i-1);

    //��ȡ����
    _mysql_psw=stream.readLine();
    i=_mysql_psw.indexOf(":");
    j=_mysql_psw.indexOf(";");
    mysql_psw=_mysql_psw.mid(i+1,j-i-1);

    //��ȡ���ݿ�����
    _mysql_dbname=stream.readLine();
    i=_mysql_dbname.indexOf(":");
    j=_mysql_dbname.indexOf(";");
    mysql_dbname=_mysql_dbname.mid(i+1,j-i-1);

    _ipBackup=stream.readLine();
    i=_ipBackup.indexOf(":");
    j=_ipBackup.indexOf(";");
    ipBackup=_ipBackup.mid(i+1,j-i-1);

    configfile.close();
}

QString Config::getPort()
{
    return port;
}

QString Config::getIP()
{
    return ip;
}

QString Config::getMysql_dbname()
{
    return mysql_dbname;
}

QString Config::getMysql_psw()
{
    return mysql_psw;
}

QString Config::getMysql_user()
{
    return mysql_user;
}
QString Config::getIPBackup()
{
    return ipBackup;
}
