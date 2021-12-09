#ifndef AIDE_H
#define AIDE_H

#include <QDialog>

namespace Ui {
class Aide;
}

class Aide : public QDialog
{
    Q_OBJECT

public:
    explicit Aide(int AdresseAfficheur, QString Protocole, QString Debit, QString Longueur, QString Parite, QString BitStop, QWidget *parent = 0);
    ~Aide();

private:
    Ui::Aide *ui;
    struct configAide
    {
        int adresse;
        QString protocole;
        QString debit;
        QString longueur;
        QString parite;
        QString stop;
    };
    configAide stockageConfigAide;
    void initialiser();
    void chargerConfigAide();
    void separationAdresse(unsigned char adresse, int adresseSeparee[7]);
    void choixFormat();
    void choixVitesse();
};

#endif // AIDE_H
