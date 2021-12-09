#ifndef AFFICHEUR_ASCII_H
#define AFFICHEUR_ASCII_H
#include <QSerialPort>
#include <QIODevice>
#include <QDebug>
#include <QTimer>

using namespace std;

class afficheur_ascii
{
public:
    afficheur_ascii();

    QSerialPort port;

    bool connecter(QString nPort, int vitesse,
                   int donnees, QString parite,
                   int stop);
    void deconnecter();
    void appel();
    void telecharger();
    void telecharger_tout();
    void recevoir(QString &message);
    void recevoir_tout(QString messages[511]);
    void ecriture();
    void heure();


    struct parametres
    {
        unsigned char *adresseAfficheur;
        unsigned char *adresseMessage;
        unsigned char *donnees;
    };

    QByteArray Date;

    parametres Parametres;
};
#endif // AFFICHEUR_ASCII_H
