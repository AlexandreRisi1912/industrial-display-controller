#include "aide.h"
#include "ui_aide.h"

Aide::Aide(int AdresseAfficheur, QString Protocole, QString Debit, QString Longueur, QString Parite, QString BitStop, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Aide)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    stockageConfigAide.adresse = AdresseAfficheur;
    stockageConfigAide.protocole = Protocole;
    stockageConfigAide.debit = Debit;
    stockageConfigAide.longueur = Longueur;
    stockageConfigAide.parite = Parite;
    stockageConfigAide.stop = BitStop;

    initialiser();
    chargerConfigAide();
}

void Aide::initialiser()
{
    //Initialisation de la forme des tableaux
    ui->tableWidgetDIP2->setColumnCount(2);
    ui->tableWidgetDIP2->setRowCount(8);
    ui->tableWidgetDIP1->setColumnCount(2);
    ui->tableWidgetDIP1->setRowCount(8);
    ui->tableWidgetDIP2->verticalHeader()->hide();
    ui->tableWidgetDIP1->verticalHeader()->hide();
    ui->tableWidgetDIP2->horizontalHeader()->hide();
    ui->tableWidgetDIP1->horizontalHeader()->hide();
    ui->tableWidgetDIP2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidgetDIP1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidgetDIP2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidgetDIP1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //Définition des colonnes et de leur taille
    for(int col = 0; col < 2; col++)
    {
        ui->tableWidgetDIP2->setColumnWidth(col, 30);
        ui->tableWidgetDIP1->setColumnWidth(col, 30);
    }
    //Définition des lignes et de leur taille ainsi que des objets
    for(int lig = 0; lig < 8; lig++)
    {
        ui->tableWidgetDIP2->setItem(lig, 0, new QTableWidgetItem);
        ui->tableWidgetDIP2->setItem(lig, 1, new QTableWidgetItem);
        ui->tableWidgetDIP1->setItem(lig, 0, new QTableWidgetItem);
        ui->tableWidgetDIP1->setItem(lig, 1, new QTableWidgetItem);
        ui->tableWidgetDIP2->setRowHeight(lig, 30);
        ui->tableWidgetDIP1->setRowHeight(lig, 30);
    }
    //On désactive les tableaux pour qu'ils soient seulement visuels
    ui->tableWidgetDIP2->setDisabled(true);
    ui->tableWidgetDIP1->setDisabled(true);
}

void Aide::chargerConfigAide()
{
    //Si protocole ASCII
    if(stockageConfigAide.protocole == "ASCII")
    {
        unsigned char adresseOctet = (unsigned char)stockageConfigAide.adresse;
        int adresseBits[7];

        //Séparation de l'adresse en 7 bits
        separationAdresse(adresseOctet, adresseBits);

        //Configuration switch Adresse/Protocole
        for (int bit = 0; bit < 7; bit++)
        {
            ui->tableWidgetDIP1->item(bit, adresseBits[bit])->setBackground(Qt::black);
        }
        ui->tableWidgetDIP1->item(7,0)->setBackground(Qt::black);

        //Configuration switch Débit/Longueur/Parité/Stop
        choixFormat();

        //4ème switch réponse de l'afficheur (inutile dans le cadre de notre application)
        ui->tableWidgetDIP2->item(3,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(3,1)->setBackground(Qt::gray);

        //Configuration switch vitesse
        choixVitesse();

        //Configuration affichage normal/gras
        ui->tableWidgetDIP2->item(7,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(7,1)->setBackground(Qt::gray);
    }
    //Si protocole RTU
    if(stockageConfigAide.protocole == "RTU")
    {
        //Configuration switch Adresse/Protocole
        ui->tableWidgetDIP1->item(0,1)->setBackground(Qt::black);
        for(int ligne = 1; ligne < 7; ligne++)
        {
            ui->tableWidgetDIP1->item(ligne,0)->setBackground(Qt::black);
        }
        ui->tableWidgetDIP1->item(7,1)->setBackground(Qt::black);

        //Configuration switch Débit/Longueur/Parité/Stop
        choixFormat();

        //4ème switch réponse de l'afficheur (inutile dans le cadre de notre application)
        ui->tableWidgetDIP2->item(3,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(3,1)->setBackground(Qt::gray);

        //Configuration switch vitesse
        choixVitesse();

        //Configuration affichage normal/gras
        ui->tableWidgetDIP2->item(7,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(7,1)->setBackground(Qt::gray);

    }
    //Si protocole TCP
    if(stockageConfigAide.protocole == "TCP")
    {
        ui->tableWidgetDIP1->item(0,1)->setBackground(Qt::black);
        for(int ligne = 1; ligne < 7; ligne++)
        {
            ui->tableWidgetDIP1->item(ligne,0)->setBackground(Qt::black);
        }
        ui->tableWidgetDIP1->item(7,1)->setBackground(Qt::black);

        //Configuration switch Débit/Longueur/Parité/Stop (inutile pour TCP)
        choixFormat();

        //4ème switch réponse de l'afficheur (inutile dans le cadre de notre application)
        ui->tableWidgetDIP2->item(3,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(3,1)->setBackground(Qt::gray);

        //Configuration switch Vitesse (inutile pour TCP)
        ui->tableWidgetDIP2->item(4,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(5,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(6,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(4,1)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(5,1)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(6,1)->setBackground(Qt::gray);

        //Configuration affichage normal/gras
        ui->tableWidgetDIP2->item(7,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(7,1)->setBackground(Qt::gray);
    }
    else
    {
        return;
    }
}

void Aide::separationAdresse(unsigned char adresse, int adresseSeparee[7])
{
    for(int i=0; i<7; i++)
    {
        adresseSeparee[i] = adresse & 1;
        adresse = adresse >>1;
    }
}

void Aide::choixFormat()
{
    if(stockageConfigAide.protocole != "TCP")
    {
        if(stockageConfigAide.longueur == "7 bits")
        {
            if(stockageConfigAide.parite == "Paire")
            {
                if(stockageConfigAide.stop == "2 bits")
                {
                    ui->tableWidgetDIP2->item(0,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,0)->setBackground(Qt::black);
                }
                else
                {
                    ui->tableWidgetDIP2->item(0,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,0)->setBackground(Qt::black);
                }
            }
            else if(stockageConfigAide.parite == "Impaire")
            {
                if(stockageConfigAide.stop == "2 bits")
                {
                    ui->tableWidgetDIP2->item(0,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,0)->setBackground(Qt::black);
                }
                else
                {
                    ui->tableWidgetDIP2->item(0,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,0)->setBackground(Qt::black);
                }
            }
        }
        else
        {
            if(stockageConfigAide.parite == "Pas de parité")
            {
                if(stockageConfigAide.stop == "2 bits")
                {
                    ui->tableWidgetDIP2->item(0,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,1)->setBackground(Qt::black);
                }
                else
                {
                    ui->tableWidgetDIP2->item(0,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,1)->setBackground(Qt::black);
                }
            }
            else if(stockageConfigAide.parite == "Paire")
            {
                if(stockageConfigAide.stop == "1 bit")
                {
                    ui->tableWidgetDIP2->item(0,0)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,1)->setBackground(Qt::black);
                }
            }
            else
            {
                if(stockageConfigAide.stop == "1 bit")
                {
                    ui->tableWidgetDIP2->item(0,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(1,1)->setBackground(Qt::black);
                    ui->tableWidgetDIP2->item(2,1)->setBackground(Qt::black);
                }
            }
        }
    }
    else
    {
        ui->tableWidgetDIP2->item(0,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(1,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(2,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(0,1)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(1,1)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(2,1)->setBackground(Qt::gray);
    }
}

void Aide::choixVitesse()
{
    int vitesse = stockageConfigAide.debit.left(5).toInt();

    switch (vitesse) {
    case 1200:
        ui->tableWidgetDIP2->item(4,0)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(5,0)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(6,0)->setBackground(Qt::black);
        break;

    case 2400:
        ui->tableWidgetDIP2->item(4,1)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(5,1)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(6,0)->setBackground(Qt::black);
        break;

    case 4800:
        ui->tableWidgetDIP2->item(4,0)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(5,0)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(6,1)->setBackground(Qt::black);
        break;

    case 9600:
        ui->tableWidgetDIP2->item(4,1)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(5,0)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(6,1)->setBackground(Qt::black);
        break;

    case 19200:
        ui->tableWidgetDIP2->item(4,0)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(5,1)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(6,1)->setBackground(Qt::black);
        break;

    case 38400:
        ui->tableWidgetDIP2->item(4,1)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(5,1)->setBackground(Qt::black);
        ui->tableWidgetDIP2->item(6,1)->setBackground(Qt::black);
        break;

    default:
        ui->tableWidgetDIP2->item(4,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(5,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(6,0)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(4,1)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(5,1)->setBackground(Qt::gray);
        ui->tableWidgetDIP2->item(6,1)->setBackground(Qt::gray);
        break;
    }
}

Aide::~Aide()
{
    delete ui;
}
