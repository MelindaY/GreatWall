#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
class Config
{
public:
    Config();//构造函数
    void loadConfig(const QString& filename);//加载配置文件
    QString getPort();//获取端口
    QString getIP();//获取IP
    QString getMysql_user();//获取配置文件中的数据库用户名
    QString getMysql_psw();//获取数据库密码
    QString getMysql_dbname();//获取数据库名
private:
    QString port;//相关变量
    QString ip;
    QString mysql_user;
    QString mysql_psw;
    QString mysql_dbname;
};

#endif // CONFIG_H
