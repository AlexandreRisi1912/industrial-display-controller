#ifndef AFFICHEUR_H
#define AFFICHEUR_H

#include <QMainWindow>
#include <QtConcurrent>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTableWidget>
#include <QtWidgets>
#include "configuration.h"
#include "master.h"
#include "afficheur_ascii.h"

namespace Ui {
class Afficheur;
}

class Afficheur : public QMainWindow
{
    Q_OBJECT

public:
    explicit Afficheur(QWidget *parent = 0);
    ~Afficheur();

signals:
    void progressionTransfert(int);

public slots:
    void initialiserTable();
    void recuperationParametres();
    void menuNouveau();
    void menuOuvrir();
    void menuEnregistrer();
    void menuEnregistrerSous();
    void menuCommunication();
    void envoiModbus();
    void ajustementPremierDernier();
    void transfererMessages();
    void appelerMessage();
    void actualiserHeure();
    void mettreHeure();

private:
    Ui::Afficheur *ui;
    QString cheminExplorateur;
    QString cheminFichierCourant;
    Configuration *configuration;
    struct parametresConfiguration
    {
        QString adresse;
        QString protocole;
        QString portCOM;
        int debit;
        int longueur;
        QString parite;
        int stop;
        QString ip;
    };
    parametresConfiguration parametresEnregistres;
    QDomDocument configXML;
    QString cheminConfigXML = "config.xml";
    QTimer *ecritureMemoire = new QTimer();
    void attenteMessages(int premier, int dernier);
    void nouveauFichier();
    void ouvrirFichier(QString chemin);
    void enregistrerFichierSous(QString chemin);
    void ASCII_RTU_TCP();
    void clignotement(QString &Message, QString clignotementMessage);
};

#endif // AFFICHEUR_H
