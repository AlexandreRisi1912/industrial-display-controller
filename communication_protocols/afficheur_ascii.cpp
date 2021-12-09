#include "afficheur_ascii.h"

afficheur_ascii::afficheur_ascii()
{

}

bool afficheur_ascii::connecter(QString nPort, int vitesse,
                                int donnees, QString parite,
                                int stop) // méthode de connexion
{
    port.setPortName(nPort);    // définir le port utilisé

    switch (vitesse) {          // définir la vitesse de connexion
    case 1200:
        port.setBaudRate(QSerialPort::Baud1200);
        break;
    case 2400:
        port.setBaudRate(QSerialPort::Baud2400);
        break;
    case 4800:
        port.setBaudRate(QSerialPort::Baud4800);
        break;
    case 9600:
        port.setBaudRate(QSerialPort::Baud9600);
        break;
    case 19200:
        port.setBaudRate(QSerialPort::Baud19200);
        break;
    case 38400:
        port.setBaudRate(QSerialPort::Baud38400);
        break;
    default:
        port.setBaudRate(QSerialPort::Baud9600);
        break;
    }

    switch (donnees) {          // définir le nombre de bit utilisé
    case 7:
        port.setDataBits(QSerialPort::Data7);
        break;
    case 8:
        port.setDataBits(QSerialPort::Data8);
        break;
    default:
        port.setDataBits(QSerialPort::Data8);
        break;
    }

    if(parite == "paire")       // définir la bit de parité
        port.setParity(QSerialPort::EvenParity);
    else if(parite == "impaire")
        port.setParity(QSerialPort::OddParity);
    else
        port.setParity(QSerialPort::NoParity);

    switch (stop) {             // définir le bit de stop
    case 1:
        port.setStopBits(QSerialPort::OneStop);
        break;
    case 2:
        port.setStopBits(QSerialPort::TwoStop);
        break;
    default:
        port.setStopBits(QSerialPort::OneStop);
        break;
    }

    port.setFlowControl(QSerialPort::NoFlowControl);// définir le flux
    if(!port.open(QIODevice::ReadWrite))
    {
            qDebug() << "Port non ouvert" << endl; // si retourne faux port non ouvert
            return false;
    }
    else
    {
           qDebug() << "Port ouvert" << endl; // si retourne vrai port ouvert
           return true;
    }
}

void afficheur_ascii::deconnecter()
{
    port.close();
}

void afficheur_ascii::appel() // méthode d'appel d'un message dans la mémoire de l'afficheur
{
   int i;
   unsigned char f[9];

   f[0] = 0x01; // Start of Heading (début de la trame)

   for(i=0; i<3; i++)
   {
       f[i+1] = Parametres.adresseAfficheur[i]; // adresse de l'afficheur ex : 0 0 1
   }

   f[4] = 0x1D; // Group Separator

   for(i=0; i<3; i++)
   {
       f[i+5] = Parametres.adresseMessage[i]; // numéro du message ex : 0 0 1
   }

   f[8] = 0x04; // End Of Transmission (fin de la transmission)

   QByteArray trame = QByteArray((char *)f, 9);
   if(port.clear(QSerialPort::AllDirections))
   {
       port.write(trame);
       port.waitForBytesWritten(3000);
   }
}

void afficheur_ascii::telecharger()
{
    int i;
    unsigned char f[12];

    f[0] = 0x05;

    for(i=0; i<3; i++)
    {
        f[i+1] = Parametres.adresseAfficheur[i];
    }

    f[4] = 0x1D;

    f[5] = 0x1B;

    f[6] = 0x46;

    f[7] = 0x33;

    for (i=0; i<3; i++)
    {
        f[i+8] = Parametres.adresseMessage[i];
    }

    f[11] = 0x06;

    QByteArray trame = QByteArray((char *)f, 12);
    if(port.clear(QSerialPort::AllDirections))
    {
        port.write(trame);
        port.waitForBytesWritten(3000);
    }
}

void afficheur_ascii::telecharger_tout()
{
    int i;
    unsigned char f[12];

    f[0] = 0x05;

    for (i=0; i<3; i++)
    {
        f[i+1] = Parametres.adresseAfficheur[i];
    }

    f[4] = 0x1D;

    f[5] = 0x1B;

    f[6] = 0x46;

    f[7] = 0x34;

    f[8] = 0x2D;

    f[9] = 0x2D;

    f[10] = 0x2D;

    f[11] = 0x06;

    QByteArray trame = QByteArray((char *)f, 12);
    if(port.clear(QSerialPort::AllDirections))
    {
        port.write(trame);
        port.waitForBytesWritten(3000);
    }
}

void afficheur_ascii::recevoir(QString &message)
{

    QByteArray lecture;
    lecture.append(port.readAll());
    for(int position= lecture.indexOf(0x1D); position < lecture.indexOf(0x1D, 25); position++)
    {
        message = message + lecture[position];
    }
    qDebug() << message;
}

void afficheur_ascii::ecriture()
{
    int i;
    unsigned char h[50];

    h[0] = 0x02;

    for (i=0; i<3; i++)
    {
        h[i+1] = Parametres.adresseAfficheur[i];
    }

    h[4] = 0x1D;

    for (i=0; i<3; i++)
    {
        h[i+5] = Parametres.adresseMessage[i];
    }

    h[8] = 0x1D;

    for (i=0; i<40; i++)
    {
        h[i+9] = Parametres.donnees[i];
    }

    h[49] = 0x03;

    QByteArray trame = QByteArray((char *)h, 50);
    if(port.clear(QSerialPort::AllDirections))
    {
        port.write(trame);
        port.waitForBytesWritten(3000);
    }
}

void afficheur_ascii::heure()
{
    QByteArray trame;
    trame += 0x05;
    trame += Date;
    trame += 0x06;

    if(port.clear(QSerialPort::AllDirections))
    {
        port.write(trame);
        port.waitForBytesWritten(3000);
    }
}
