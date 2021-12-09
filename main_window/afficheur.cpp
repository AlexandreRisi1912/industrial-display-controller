#include "afficheur.h"
#include "ui_afficheur.h"

Afficheur::Afficheur(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Afficheur)
{
    ui->setupUi(this);
    initialiserTable();
    recuperationParametres();
    connect(ui->actionNouveau, SIGNAL(triggered(bool)), this, SLOT(menuNouveau()));
    connect(ui->actionOuvrir, SIGNAL(triggered(bool)), this, SLOT(menuOuvrir()));
    connect(ui->actionEnregistrer, SIGNAL(triggered(bool)), this, SLOT(menuEnregistrer()));
    connect(ui->actionEnregistrer_sous, SIGNAL(triggered(bool)), this, SLOT(menuEnregistrerSous()));
    connect(ui->actionCommunication, SIGNAL(triggered(bool)), this, SLOT(menuCommunication()));
    connect(ui->pushButtonEnvoiRTUTCP, SIGNAL(clicked(bool)), this, SLOT(envoiModbus()));
    connect(ui->spinBoxTransfertPremier, SIGNAL(valueChanged(int)), this, SLOT(ajustementPremierDernier()));
    connect(ui->pushButtonTransfertEnvoi, SIGNAL(clicked(bool)), this, SLOT(transfererMessages()));
    connect(ui->pushButtonAppelEnvoi, SIGNAL(clicked(bool)), this, SLOT(appelerMessage()));
    connect(ui->pushButtonHeureEnvoi, SIGNAL(clicked(bool)), this, SLOT(mettreHeure()));
    connect(ui->pushButtonActualiserHeure, SIGNAL(clicked(bool)), this, SLOT(actualiserHeure()));
}

void Afficheur::initialiserTable()
{
    //Initialisation des variables
    int nbLignes = 1533;
    int nbColonnes = 2;
    int nbMessages = 511;
    QStringList titresColonnes = {"Affichage", "Clignotement"};
    QStringList titresLignes;

    //Vérification de l'OS pour Windows 10 (Problème d'affichage du tableau)
    if(QSysInfo::windowsVersion()==QSysInfo::WV_WINDOWS10)
    {
        ui->tableEditionMessages->setStyleSheet("QHeaderView::section{"
                                                "border-top:0px solid #D8D8D8;"
                                                "border-left:0px solid #D8D8D8;"
                                                "border-right:1px solid #D8D8D8;"
                                                "border-bottom: 1px solid #D8D8D8;"
                                                "background-color:white;"
                                                "padding:4px;"
                                            "}"
                                            "QTableCornerButton::section{"
                                                "border-top:0px solid #D8D8D8;"
                                                "border-left:0px solid #D8D8D8;"
                                                "border-right:1px solid #D8D8D8;"
                                                "border-bottom: 1px solid #D8D8D8;"
                                                "background-color:white;"
                                            "}");
    }

    //Fixation des entête pour désactiver le changement de taille
    ui->tableEditionMessages->horizontalHeader()->setSectionResizeMode (QHeaderView::Fixed);
    ui->tableEditionMessages->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);

    //Ajout des colonnes et lignes du tableau
    ui->tableEditionMessages->setColumnCount(nbColonnes);
    ui->tableEditionMessages->setRowCount(nbLignes);

    //Ajout des entêtes horizontales du tableau
    ui->tableEditionMessages->setHorizontalHeaderLabels(titresColonnes);

    //Boucle d'initialisation de chaque cellule
    for(int i = 0; i < nbMessages; i++)
    {
        QString ligne = QString::number(i+1);
        titresLignes << "Message  " << " Ligne 1" << " Ligne 2";
        ui->tableEditionMessages->horizontalHeader()->setSectionsClickable(false);
        if((i*3) < nbLignes)
        {
            //Si c'est une cellule avec un numéro de message
            ui->tableEditionMessages->setItem((i*3), 0, new QTableWidgetItem(ligne));
            ui->tableEditionMessages->setSpan((i*3), 0, 1, 2);
            ui->tableEditionMessages->item((i*3), 0)->setForeground(QColor(255, 255, 255, 255));
            ui->tableEditionMessages->item((i*3), 0)->setBackground(QColor(80, 80, 80, 255));
            ui->tableEditionMessages->item((i*3), 0)->setTextAlignment(Qt::AlignCenter);
            ui->tableEditionMessages->item((i*3), 0)->setFlags(Qt::NoItemFlags);
        }
        if((i*3)+1 < nbLignes || (i*3)+2 < nbLignes)
        {
            //Si ce sont les cellules de données
            ui->tableEditionMessages->setItem((i*3)+1, 0, new QTableWidgetItem);
            ui->tableEditionMessages->setItem((i*3)+1, 1, new QTableWidgetItem);
            ui->tableEditionMessages->setItem((i*3)+2, 0, new QTableWidgetItem);
            ui->tableEditionMessages->setItem((i*3)+2, 1, new QTableWidgetItem);
        }
    }
    //Ajout des entêtes verticales
    ui->tableEditionMessages->setVerticalHeaderLabels(titresLignes);

    //Force les en-têtes à s'afficher
    ui->tableEditionMessages->horizontalHeader()->setVisible(true);
    ui->tableEditionMessages->verticalHeader()->setVisible(true);
    QFont font = QFont("Consolas",14);
    ui->tableEditionMessages->horizontalHeader()->setFont(font);
    ui->tableEditionMessages->verticalHeader()->setFont(font);
}

void Afficheur::recuperationParametres()
{
    //Crétion du fichier
    QFile fichier(cheminConfigXML);

    //Si fichier non ouvert -> valeurs par défaut + fermeture méthode
    if(!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Erreur à l'ouverture du fichier", "Le document XML n'a pas pu être ouvert.");
        parametresEnregistres.adresse = "001";
        parametresEnregistres.protocole = "ASCII";
        parametresEnregistres.portCOM = "COM1";
        parametresEnregistres.debit = 9600;
        parametresEnregistres.longueur = 8;
        parametresEnregistres.parite = "pas de parité";
        parametresEnregistres.stop = 1;
        parametresEnregistres.ip = "192.168.0.52";
        return;
    }

    //Si fichier non lié au Document -> valeurs par défaut + fermeture méthode
    if (!configXML.setContent(&fichier))
    {
         fichier.close();
         QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
         parametresEnregistres.adresse = "001";
         parametresEnregistres.protocole = "ASCII";
         parametresEnregistres.portCOM = "COM1";
         parametresEnregistres.debit = 9600;
         parametresEnregistres.longueur = 8;
         parametresEnregistres.parite = "pas de parité";
         parametresEnregistres.stop = 1;
         parametresEnregistres.ip = "192.168.0.52";
         return;
    }
    QDomElement racine = configXML.namedItem("configuration").toElement();

    //Si racine non présente -> valeurs par défaut + fermeture méthode
    if (racine.isNull())
    {
        fichier.close();
        QMessageBox::warning(this, "Erreur lors du parcours du document XML", "<configuration> n'a pas été trouvé, chargement de la configuration par défaut.");
        parametresEnregistres.adresse = "001";
        parametresEnregistres.protocole = "ASCII";
        parametresEnregistres.portCOM = "COM1";
        parametresEnregistres.debit = 9600;
        parametresEnregistres.longueur = 8;
        parametresEnregistres.parite = "pas de parité";
        parametresEnregistres.stop = 1;
        parametresEnregistres.ip = "192.168.0.52";
        return;
    }
    QDomNode configuration = racine.firstChild();

    //Boucle de parcours du document
    for (; !configuration.isNull(); configuration = configuration.nextSibling())
    {
        QDomElement parametres = configuration.toElement();
        if (parametres.tagName() == "adresse")
        {
            if (parametres.isNull() || parametres.text() == "")
            {
                parametresEnregistres.adresse = "001";
            }
            else
            {
                parametresEnregistres.adresse = parametres.text().rightJustified(3,'0');
            }
        }
        if (parametres.tagName() == "protocole")
        {
            if (parametres.isNull() || parametres.text() == "")
            {
                parametresEnregistres.protocole = "ASCII";
            }
            else
            {
                switch (parametres.text().toInt()) {
                case 0:
                    parametresEnregistres.protocole = "ASCII";
                    break;
                case 1:
                    parametresEnregistres.protocole = "RTU";
                    break;
                case 2:
                    parametresEnregistres.protocole = "TCP";
                    break;
                default:
                    parametresEnregistres.protocole = "ASCII";
                    break;
                }
            }
        }
        if (parametres.tagName() == "serie")
        {
            QDomElement paramSerie;
            paramSerie = parametres.firstChildElement("port");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                parametresEnregistres.portCOM = "COM1";
            }
            else
            {
                parametresEnregistres.portCOM = QString::number(paramSerie.text().toInt()+1);
            }
            paramSerie = parametres.firstChildElement("debit");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                parametresEnregistres.debit = 9600;
            }
            else
            {
                switch (paramSerie.text().toInt()) {
                case 0:
                    parametresEnregistres.debit = 1200;
                    break;
                case 1:
                    parametresEnregistres.debit = 2400;
                    break;
                case 2:
                    parametresEnregistres.debit = 4800;
                    break;
                case 3:
                    parametresEnregistres.debit = 9600;
                    break;
                case 4:
                    parametresEnregistres.debit = 19200;
                    break;
                case 5:
                    parametresEnregistres.debit = 38400;
                    break;
                default:
                    parametresEnregistres.debit = 9600;
                    break;
                }
            }
            paramSerie = parametres.firstChildElement("longueur");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                parametresEnregistres.longueur = 8;
            }
            else
            {
                switch (paramSerie.text().toInt()) {
                case 0:
                    parametresEnregistres.longueur = 7;
                    break;
                case 1:
                    parametresEnregistres.longueur = 8;
                    break;
                default:
                    parametresEnregistres.longueur = 8;
                    break;
                }
            }
            paramSerie = parametres.firstChildElement("parite");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                parametresEnregistres.parite = "pas de parité";
            }
            else
            {
                switch (paramSerie.text().toInt()) {
                case 0:
                    parametresEnregistres.parite = "paire";
                    break;
                case 1:
                    parametresEnregistres.parite = "impaire";
                    break;
                case 2:
                    parametresEnregistres.parite = "pas de parité";
                    break;
                default:
                    parametresEnregistres.parite = "pas de parité";
                    break;
                }
            }
            paramSerie = parametres.firstChildElement("stop");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                parametresEnregistres.stop = 1;
            }
            else
            {
                switch (paramSerie.text().toInt()) {
                case 0:
                    parametresEnregistres.stop = 1;
                    break;
                case 1:
                    parametresEnregistres.stop = 2;
                    break;
                default:
                    parametresEnregistres.stop = 1;
                    break;
                }
            }
        }
        if (parametres.tagName() == "ethernet")
        {
            QDomElement paramEthernet;
            paramEthernet = parametres.firstChildElement("IP");
            if (paramEthernet.isNull() || paramEthernet.text() == "")
            {
                parametresEnregistres.ip = "192.168.0.52";
            }
            else
            {
                parametresEnregistres.ip = paramEthernet.text();
            }
        }
    }
    fichier.close();
    ASCII_RTU_TCP();
}

void Afficheur::menuNouveau()
{
    //Récupération du chemin
    cheminExplorateur = QFileDialog::getSaveFileName(this,
    tr("Nouveau fichier"), cheminExplorateur,
    tr("Fichier Afficheur (*.aimlp)"));

    //Si chemin a été choisi
    if(cheminExplorateur != "")
    {
        nouveauFichier();
        cheminFichierCourant = cheminExplorateur;
    }
}

void Afficheur::menuOuvrir()
{
    //Récupération du chemin
    cheminExplorateur = QFileDialog::getOpenFileName(this,
    tr("Ouvrir fichier"), cheminExplorateur,
    tr("Fichier Afficheur (*.aimlp)"));

    //Si chemin a été choisi
    if(cheminExplorateur != "")
    {
        ouvrirFichier(cheminExplorateur);
        cheminFichierCourant = cheminExplorateur;
    }
}

void Afficheur::menuEnregistrer()
{
    //Si un chemin est déjà présent
    if(cheminExplorateur != "")
        enregistrerFichierSous(cheminExplorateur);
    else //Sinon appel pour enregistrement
        menuEnregistrerSous();
}

void Afficheur::menuEnregistrerSous()
{
    //Récupération du chemin
    cheminExplorateur = QFileDialog::getSaveFileName(this,
    tr("Enregistrer fichier"), cheminExplorateur,
    tr("Fichier Afficheur (*.aimlp)"));

    //Si chemin a été choisi
    if(cheminExplorateur != "")
    {
        enregistrerFichierSous(cheminExplorateur);
        cheminFichierCourant = cheminExplorateur;
    }
}

void Afficheur::menuCommunication()
{
    //Création dynamique d'un objet de type Configuration
    configuration = new Configuration(this);
    configuration->move(this->x() + 25, this->y() + 150);
    configuration->show();
}

void Afficheur::envoiModbus()
{
    Master *Modbus = new Master();
    int rtu_tcp = 1;
    int numeroMessage = ui->spinBoxMessageRTUTCP->value();
    QString partie1 = ui->tableEditionMessages->item(((numeroMessage-1)*3) + 1, 0)->text();
    QString partie2 = ui->tableEditionMessages->item(((numeroMessage-1)*3) + 2, 0)->text();
    QString partie1Clignotement = ui->tableEditionMessages->item(((numeroMessage-1)*3) + 1, 1)->text();
    QString partie2Clignotement = ui->tableEditionMessages->item(((numeroMessage-1)*3) + 2, 1)->text();
    partie1.resize(12, ' ');
    partie2.resize(12, ' ');
    partie1Clignotement.resize(12, ' ');
    partie2Clignotement.resize(12, ' ');
    QString Message = partie1 + partie2;
    QString clignotementMessage = partie1Clignotement + partie2Clignotement;
    clignotement(Message, clignotementMessage);

    if(parametresEnregistres.protocole == "TCP")
        rtu_tcp = 2;
    else
        rtu_tcp = 1;
    Modbus->Modbus(rtu_tcp, Message,
                  "COM" + parametresEnregistres.portCOM,
                  parametresEnregistres.debit,
                  parametresEnregistres.parite,
                  parametresEnregistres.longueur,
                  parametresEnregistres.stop,
                  502,
                  parametresEnregistres.ip);
}

void Afficheur::ajustementPremierDernier()
{
    if(ui->spinBoxTransfertDernier->value() < ui->spinBoxTransfertPremier->value())
        ui->spinBoxTransfertDernier->setValue(ui->spinBoxTransfertPremier->value());
}

void Afficheur::transfererMessages()
{
    afficheur_ascii *ASCII = new afficheur_ascii();
    if(ASCII->connecter("COM" + parametresEnregistres.portCOM,
                        parametresEnregistres.debit, parametresEnregistres.longueur,
                        parametresEnregistres.parite, parametresEnregistres.stop))
    {
        ASCII->Parametres.adresseAfficheur = (unsigned char*)qstrdup(parametresEnregistres.adresse.toLatin1().constData());
        int nMessage = ui->spinBoxTransfertPremier->value();
        int nMessageMax = ui->spinBoxTransfertDernier->value();
        QProgressDialog progression("Transmission des messages : ", "Annuler", 0, nMessageMax-nMessage+1, this);
        progression.setWindowModality(Qt::WindowModal);
        progression.setWindowFlags(progression.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        progression.show();
        int prog = 0;
        int max = nMessageMax-nMessage+1;
        QString label = "Transmission des messages : " + QString::number(prog) + "/" + QString::number(max);
        progression.setLabelText(label);
        QApplication::processEvents();
        for(; nMessage <= nMessageMax;)
        {
            if(ecritureMemoire->remainingTime() == -1 || ecritureMemoire->remainingTime() == 0)
            {
                prog++;
                QString adresseMessage = QString::number(nMessage).rightJustified(3,'0');
                ASCII->Parametres.adresseMessage = (unsigned char*)qstrdup(adresseMessage.toLatin1().constData());
                QString partie1 = ui->tableEditionMessages->item(((nMessage-1)*3) + 1, 0)->text();
                QString partie2 = ui->tableEditionMessages->item(((nMessage-1)*3) + 2, 0)->text();
                QString partie1Clignotement = ui->tableEditionMessages->item(((nMessage-1)*3) + 1, 1)->text();
                QString partie2Clignotement = ui->tableEditionMessages->item(((nMessage-1)*3) + 2, 1)->text();
                partie1.resize(12, ' ');
                partie2.resize(12, ' ');
                partie1Clignotement.resize(12, ' ');
                partie2Clignotement.resize(12, ' ');
                QString Message = partie1 + partie2;
                QString clignotementMessage = partie1Clignotement + partie2Clignotement;
                clignotement(Message, clignotementMessage);
                Message.resize(40, ' ');

                if(progression.wasCanceled())
                    break;

                ASCII->Parametres.donnees = (unsigned char*)qstrdup(Message.toLatin1().constData());
                qDebug() << ASCII->Parametres.donnees;
                ASCII->ecriture();
                ecritureMemoire->start(1000);
                nMessage++;
                progression.setValue(prog);
                label = "Transmission des messages : " + QString::number(prog) + "/" + QString::number(max);
                progression.setLabelText(label);
                progression.show();
                QApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
            }
        }
        QApplication::processEvents();
        if(ui->checkBoxTransfertAppel->isChecked())
        {
            QApplication::processEvents();
            ecritureMemoire->start(1000);
            bool pret = false;
            while(pret == false)
            {
                if(ecritureMemoire->remainingTime() == -1 || ecritureMemoire->remainingTime() == 0)
                {
                    pret = true;
                    QString adresseMessage = QString::number(ui->spinBoxTransfertAppel->value()).rightJustified(3, '0');
                    ASCII->Parametres.adresseMessage = (unsigned char*)qstrdup(adresseMessage.toLatin1().constData());
                    ASCII->appel();
                    ecritureMemoire->start(1000);
                    while(ecritureMemoire->remainingTime() != 0);
                }
            }
        }
        ASCII->deconnecter();
    }
    else
    {
        QMessageBox::warning(this, "Erreur à l'ouverture", "Le port COM"+parametresEnregistres.portCOM+" n'a pas pu être ouvert.");
    }
}

void Afficheur::appelerMessage()
{
    afficheur_ascii *ASCII = new afficheur_ascii();
    if(ASCII->connecter("COM" + parametresEnregistres.portCOM,
                        parametresEnregistres.debit, parametresEnregistres.longueur,
                        parametresEnregistres.parite, parametresEnregistres.stop))
    {
        ASCII->Parametres.adresseAfficheur = (unsigned char*)qstrdup(parametresEnregistres.adresse.toLatin1().constData());
        QString adresseMessage = QString::number(ui->spinBoxAppelMessage->value()).rightJustified(3, '0');
        ASCII->Parametres.adresseMessage = (unsigned char*)qstrdup(adresseMessage.toLatin1().constData());
        ASCII->appel();
        ecritureMemoire->start(1000);
        while(ecritureMemoire->remainingTime() != 0);
        ASCII->deconnecter();
    }
    else
    {
        QMessageBox::warning(this, "Erreur à l'ouverture", "Le port COM"+parametresEnregistres.portCOM+" n'a pas pu être ouvert.");
    }
}

void Afficheur::actualiserHeure()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

void Afficheur::mettreHeure()
{
    if(ui->checkBoxActualiserEnvoi->isChecked())
        actualiserHeure();
    afficheur_ascii *ASCII = new afficheur_ascii();
    if(ASCII->connecter("COM" + parametresEnregistres.portCOM,
                        parametresEnregistres.debit, parametresEnregistres.longueur,
                        parametresEnregistres.parite, parametresEnregistres.stop))
    {
        QApplication::processEvents();
        QString dateHeure = ui->dateTimeEdit->dateTime().toString("HHmmssMMddyy");
        dateHeure += QString::number(ui->dateTimeEdit->date().dayOfWeek()-1);
        ASCII->Date = dateHeure.toUtf8();
        ASCII->heure();
        ecritureMemoire->start(1000);
        while(ecritureMemoire->remainingTime() != 0);
        ASCII->deconnecter();
    }

}

void Afficheur::nouveauFichier()
{
    initialiserTable();
}

void Afficheur::ouvrirFichier(QString chemin)
{
    //Initialisation du fichier
    QString donnee;
    QFile fichier(chemin);
    QStringList ligne, donneeCase;

    //Compteur de case numéro de message
    int nbSeparateurs=0;

    //Test d'ouverture du fichier
    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        donnee = fichier.readAll();
        ligne = donnee.split("\n");
        fichier.close();
    }

    //Boucle de parcours des données ligne par ligne
    for(int x = 0; x < ligne.size(); x++)
    {
        //Si numéro message -> passe une ligne
        if(x%2 == 0)
            nbSeparateurs++;
        donneeCase = ligne.at(x).split(";");
        //Boucle de parcours par colonne
        for (int y = 0; y < donneeCase.size(); y++)
        {
            //Si il y a un objet et que la donnée n'est pas vide
            if(ui->tableEditionMessages->item(x+nbSeparateurs,y) && donneeCase[y] != " ")
                ui->tableEditionMessages->item(x+nbSeparateurs,y)->setText(donneeCase[y]);
            else //Sinon on créer un nouvel objet
                ui->tableEditionMessages->setItem(x+nbSeparateurs,y,new QTableWidgetItem);
        }
    }
}

void Afficheur::enregistrerFichierSous(QString chemin)
{
    //Initialisation du fichier
    QFile f(chemin);

    //Si le fichier s'ouvre
    if (f.open(QFile::WriteOnly | QFile::Truncate))
    {
        //Création du flux
        QTextStream donnees( &f );
        QStringList liste;

        //Parcours des données à enregistrer ligne par ligne
        for( int r = 0; r < ui->tableEditionMessages->rowCount(); r++ )
        {
            liste.clear();
            //Si ce n'est pas un numéro de message
            if(ui->tableEditionMessages->verticalHeaderItem(r)->text() != "Message  ")
            {
                //Parcours de données par colonne
                for( int c = 0; c < ui->tableEditionMessages->columnCount(); c++ )
                {
                    //Si il y a un objet non vide
                    if(ui->tableEditionMessages->item( r, c ))
                        liste << ui->tableEditionMessages->item( r, c )->text();
                    else //Sinon envoi d'un blanc
                        liste << " ";
                }
                //Joint les données par des ";" et des sauts de lignes
                donnees << liste.join( ";" )+"\n";
            }
        }
        f.close();
    }
}

void Afficheur::ASCII_RTU_TCP()
{
    if(parametresEnregistres.protocole == "ASCII")
    {
        ui->groupBoxASCII->setEnabled(true);
        ui->groupBoxRTU_TCP->setEnabled(false);
    }
    else
    {
        ui->groupBoxASCII->setEnabled(false);
        ui->groupBoxRTU_TCP->setEnabled(true);
        if(parametresEnregistres.protocole == "RTU")
            ui->groupBoxRTU_TCP->setTitle("Modbus RTU");
        if(parametresEnregistres.protocole == "TCP")
            ui->groupBoxRTU_TCP->setTitle("Modbus TCP");
    }
}

void Afficheur::clignotement(QString &Message, QString clignotementMessage)
{
    for(int nChar = 0; nChar < 24; nChar++)
    {
        if(/*Message[nChar] != ' ' &&*/ clignotementMessage[nChar] != ' ')
        {
            char caractere = Message[nChar].toLatin1();
            caractere += 0x80;
            Message[nChar] = QChar(caractere);
        }
    }
}

Afficheur::~Afficheur()
{
    delete ui;
}
