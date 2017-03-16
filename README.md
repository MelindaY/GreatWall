﻿# README

[TOC]

项目主要有两个部分：

* **服务器接收端**，*项目名：长城保存状态智能感知关键技术研究*
* **无源感知模块**，*项目名：高鲁棒性的无源感知网络多协议共存与转换机制研究*

---
##1. 服务器接收端

### 项目简介

位于陕北榆林的明长城受当地较为恶劣的自然环境（强风，酸雨等）影响，风化严重。项目组主要通过部署无线传感器，采集长城土壤相关指标（土壤盐分，土壤湿度，土壤，降雨量，风速，风向和温湿度），以实现长城远程监控和防范的目的。


### 接收端简介

#### 具体模块
* **config：** 提取配置文件信息，如网关端口，数据库名等
* **dbServer:** 数据库增删改查
* **main:** 主程序入口
* **mainWindow:** 界面设计
* **myServer:** 连接数据库，监听端口（TCP/IP）
* **myThread:** 解析数据，存储并记录异常数据
* **tool:** 传感器解析算法（进制转换，BCD码转换）

#### 相关技术及语言
* 设计模式：适配器模式
* 网络编程：套接字
* 多线程
* IO操作
* MySQL, C++, Qt

####版本介绍
**v1.0** 原始TCP版，主要负责tool文件中解析方式修改，根据新的包格式收取数据
**v1.1** UDP版，主要负责重新搭建框架，创建UDP Socket 
**v1.2** 状态机版，主要根据不同的输入转到不同状态进行处理
TCP测试版：模拟网关进行数据发送，测试接收端稳定性和准确性

## 2. 能量收集模块
### 项目简介

利用无源感知和背反射技术，解决传统传感器网络电池能量供给不足的问题，实现2.4GHz下跨协议（蓝牙，WiFi，zigBee）通信。

### 能量收集模块简介

空气中RF信号可用于低耗电模块的供能。

### 主要原理

该模块简而言之为倍压整流电路。采用高增益天线接收空气中RF信号，肖特基二极管将原有的AC信号转换为DC信号，后利用电容进行电量存储。


