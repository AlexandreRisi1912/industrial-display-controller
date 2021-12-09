#include "master.h"

Master::Master()
{

}

void Master::RTU(QString com,int baud,QString pariteS,int datab,int stop)  //si le choix est le mode RTU
{
    int parite;
    if( pariteS=="paire")
        parite=2;
    else if(pariteS=="impaire")
        parite=3;
    else
        parite=0;
    modbusDevice = new QModbusRtuSerialMaster;  //Représente un client Modbus qui utilise un
                                                //bus série pour sa communication avec le serveur Modbus
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,com);// détermine le port utilisé
    modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,parite);// determine la parité
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,baud);// fixe la vitesse
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,datab);// contient les bits de données dans une trame
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,stop);// fixe le bit de stop
}

void Master::TCP(int port, QString ip) // si le choix est le mode TCP
{
    //permet de récupérer le char* contenu dans un objet de type string
    modbusDevice = new QModbusTcpClient; // classe d'interface pour le périphérique client Modbus TCP
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);//fixe le n° du port
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);// fixe l'adresse Ip de l'afficheur
}

void Master::Reponse()
{
    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 1)) {// envoie de la trame
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    if (reply->error() == QModbusDevice::NoError) {// si une erreur est a declaré ou non lors de la reception de la trame
                        qDebug() << "Aucune erreur a declare";
                    } else if (reply->error() == QModbusDevice::ReadError){
                        qDebug() << "Erreur lors de la lecture";
                    } else if (reply->error() == QModbusDevice::WriteError){
                        qDebug() << "Erreur lors de l'ecriture";
                    } else if (reply->error() == QModbusDevice::ConnectionError){
                        qDebug() << "Erreur lors de la connection";
                    } else if (reply->error() == QModbusDevice::ConfigurationError){
                        qDebug() << "Erreur de configuration";
                    } else if (reply->error() == QModbusDevice::ProtocolError){
                        qDebug() << "Erreur de protocole";
                    } else if (reply->error() == QModbusDevice::UnknownError){
                        qDebug() << "Erreur inconnu";
                    } else if (reply->error() == QModbusDevice::UnknownError){
                        qDebug() << "La réponse a été interrompue en raison d'une déconnexion du périphérique";
                    }
                    reply->deleteLater();
                });
            } else
                // broadcast replies return immediately
                reply->deleteLater();
        } else
            qDebug() << "Trame non envoye";
}

void Master::Modbus(int choix, QString bytes, QString com, int baud, QString pariteS, int datab, int stop, int port, QString ip)
{
    if(choix==1)
        RTU(com, baud, pariteS,datab,stop);
    else if(choix==2)
        TCP(port, ip);

    int z = 0;

    QByteArray trame[12]={"  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "}; // Pour eviter des caracteres indesirable sur
                                                                                                  //l'afficheur la trame est remplie d'éspace


   int taille = bytes.size(); // recupere la taille du message envoyé

    for(int i=0;i<bytes.length();i=i+2)
    {

        QString bytes2=bytes.mid(i,2);
        trame[z]=bytes2.toUtf8();// Renvoie une représentation UTF-8 de la chaîne en tant que QByteArray.
        z++;
    }

    int pouip = taille % 2 ;

    if(pouip != 0) // si le message est impaire, integre un espace a la fin
    {              //pour la rendre paire et éviter un mauvais caractere sur l'afficheur
        QString bytes2=bytes.mid(taille-1,2);
        bytes2=bytes2+" ";
        trame[z-1]=bytes2.toUtf8();
    }



    quint16 t = qFromBigEndian<quint16>((uchar*)trame[0].data());//permet la separation de la trame en parties
    quint16 ta = qFromBigEndian<quint16>((uchar*)trame[1].data());// de deux octets pour pouvoir l'envoyer grace
    quint16 tb = qFromBigEndian<quint16>((uchar*)trame[2].data());// a la fonction append
    quint16 tc = qFromBigEndian<quint16>((uchar*)trame[3].data());
    quint16 td = qFromBigEndian<quint16>((uchar*)trame[4].data());
    quint16 te = qFromBigEndian<quint16>((uchar*)trame[5].data());
    quint16 tf = qFromBigEndian<quint16>((uchar*)trame[6].data());
    quint16 tg = qFromBigEndian<quint16>((uchar*)trame[7].data());
    quint16 th = qFromBigEndian<quint16>((uchar*)trame[8].data());
    quint16 ti = qFromBigEndian<quint16>((uchar*)trame[9].data());
    quint16 tj = qFromBigEndian<quint16>((uchar*)trame[10].data());
    quint16 tk = qFromBigEndian<quint16>((uchar*)trame[11].data());

    QVector <quint16> values;
    values.append(t); // utilisé pour ajouter une chaîne à la fin d'une autre chaîne
    values.append(ta);
    values.append(tb);
    values.append(tc);
    values.append(td);
    values.append(te);
    values.append(tf);
    values.append(tg);
    values.append(th);
    values.append(ti);
    values.append(tj);
    values.append(tk);


    writeUnit.setRegisterType(QModbusDataUnit::HoldingRegisters);//réglage du code fonction
    writeUnit.setStartAddress(1);//Adresse de depart
    writeUnit.setValueCount(values.size());//nombre de mots à forcer
    writeUnit.setValues(values);// nombre d'octets

    connect(modbusDevice, &QModbusClient::stateChanged, this, &Master::onStateChanged);//connexion à l'afficheur

    if (!modbusDevice->connectDevice()) {
        qDebug() << "connexion impossible: " << modbusDevice->errorString();
    }

}



void Master::onStateChanged(QModbusDevice::State state)
{
    switch (state)
    {   //etat actuelle de la connexion
        case QModbusDevice::UnconnectedState: qDebug() << "Déconnecté"; break;
        case QModbusDevice::ConnectingState: qDebug() << "En connexion"; break;
        case QModbusDevice::ConnectedState:
                qDebug() << "Connecté";
                Reponse();
                break;
        case QModbusDevice::ClosingState: qDebug() << "Fermeture"; break;
    }

}
