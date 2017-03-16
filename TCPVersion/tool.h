#ifndef TOOL_H
#define TOOL_H

#include <QtCore>
#define SIZE 1024
void unlockPocket_TemHumLight(char buffer[SIZE],int datasize,int &nodeID,double &fVoltage,double &fLight,double &fTemperature, double &fHumidity);// 解析数据包
void unlockPocket_hop(char buffer[SIZE],int datasize,int &nodeID,int &hopNum,int hop[5],int &PacketNum);//解析控制包
void unlockPocket_rain(char buffer[SIZE],int datasize,int &nodeID,int &rainnode_addr,double &tem_outside,double &tem_inside,double &ph,double &rainfall,double &windspeed,double &winddirection,double &powervoltage,int &rain,double &fVoltage,double &voltagePre,int &emptyTimes);
void unlockPocket_sioltemhum(char buffer[SIZE],int &nodeID,int &addrCode1,int &addrCode2,double &tem_30cm,double &hum_30cm,double &tem_15cm,double &hum_15cm,double &tem_5cm,double &hum_5cm,double &fVoltage,double parameters[150][14],int count);
void unlockPocket_salt(char buffer[SIZE],int datasize,int &nodeID,int &saltaddrcode,double &tem,double &hum,double &verticalSalt,double &surfaceSalt,double &fVoltage,double saltpara[109][2],int count);
int switchNum(char ch);//十六进制转换成十进制
int unlockPocket_sioltemphumiTest(char buffer[SIZE],int datasize,int &addrCode1,int &addrCode2,int &symbol_30cm,int &temp1_30cm,int &temp2_30cm,int &humi1_30cm,int &humi2_30cm,int &symbol_15cm,int &temp1_15cm,int &temp2_15cm,int &humi1_15cm,int &humi2_15cm,int &symbol_5cm,int &temp1_5cm,int &temp2_5cm,int &humi1_5cm,int &humi2_5cm,int &voltagetest,int &nodeID);
void unlockPocket_shake(char buffer[1024],int buffersize,int &nodeID,double &shake1,double &shake2,double &shake3,double &fVoltage);//解析震动传感器数据


void unlockPocket_saltTest(char buffer[1024],int datasize,int &addrCode,int &symbol_30cm,int &temp_30cm,int &humi_30cm,int &vertSalt,int &surfSalt,int &voltagetest,int &nodeid);//盐分原始数据
void unlockPocket_shakeTest(char buffer[1024],int datasize,int &shakeOne,int &shakeTwo,int &shakeThree,int &voltagetest,int &nodeid);
void unlockPocket_airHumidity(char buffer[SIZE],int datasize,int &nodeID,double &tem,double &hum);//空气温湿度
void unlockControlPac(char buffer[1024],int datasize,int &nodeID,int &countControl,double &fVoltage,double &RSSI,double &coorVoltage);
void unlockPocket_rainTest(char buffer[SIZE],int datasize,int &nodeID,int &rainnode_addr,int &tem_outside_int,int &tem_outside_dec,int &tem_inside_int,int &tem_inside_dec,int &phInt,int &phDec,int &rainInt,int &rainDec,int &windspeedInt,int &windspeedDec,int &winddirectionInt,int &winddirectionDec,int &powerInt,int &powerDec,int &rainFall, int &fVoltage);
#endif // TOOL_H
