#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
class Config
{
public:
    Config();//���캯��
    void loadConfig(const QString& filename);//���������ļ�
    QString getPort();//��ȡ�˿�
    QString getIP();//��ȡIP
    QString getMysql_user();//��ȡ�����ļ��е����ݿ��û���
    QString getMysql_psw();//��ȡ���ݿ�����
    QString getMysql_dbname();//��ȡ���ݿ���
    QString getIPBackup();//��ȡ����mysql IP��ַ
private:
    QString port;//��ر���
    QString ip;
    QString mysql_user;
    QString mysql_psw;
    QString mysql_dbname;
    QString ipBackup;//����MysqlIP
};

#endif // CONFIG_H
