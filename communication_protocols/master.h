#ifndef MASTER_H
#define MASTER_H

#include <QVector>
#include <QUrl>
#include <QVariant>
#include <QModbusRtuSerialMaster>
#include <QtSerialPort/QtSerialPort>
#include <QDebug>
#include <iostream>
#include <QString>
#include <QModbusReply>
#include <QObject>
#include <QModbusTcpClient>
#include <QModbusDataUnit>

class Master:public QObject
{
    Q_OBJECT
public:
    Master();
    void RTU(QString com, int baud, QString pariteS, int datab, int stop);
    void TCP(int port, QString ip);
    void Reponse();
    void Modbus(int choix, QString bytes,QString com,int baud,QString pariteS,int datab,int stop,int port,QString ip);

public slots:
    void onStateChanged(QModbusDevice::State state);

private:
    QModbusClient* modbusDevice;
    QModbusReply* modbusReply;
    QModbusDataUnit writeUnit;
};

#endif // MASTER_H
