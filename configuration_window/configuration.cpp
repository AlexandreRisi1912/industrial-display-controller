#include "configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    setWindowModality(Qt::ApplicationModal);
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    aide = new Aide(ui->spinBoxAdresseAfficheur->value(),
                      ui->comboBoxProtocole->currentText(),
                      ui->comboBoxDebit->currentText(),
                      ui->comboBoxLongueur->currentText(),
                      ui->comboBoxParite->currentText(),
                      ui->comboBoxBitStop->currentText(),
                      this);
    chargerConfig();
    optionsConfigurationParite();
    ASCII_RTU_TCP();

    connect(ui->comboBoxProtocole, SIGNAL(currentTextChanged(QString)), this, SLOT(rafraichissementAide()));
    connect(ui->comboBoxDebit, SIGNAL(currentTextChanged(QString)), this, SLOT(rafraichissementAide()));
    connect(ui->comboBoxBitStop, SIGNAL(currentTextChanged(QString)), this, SLOT(rafraichissementAide()));
    connect(ui->spinBoxAdresseAfficheur, SIGNAL(editingFinished()), this, SLOT(rafraichissementAide()));

    connect(ui->buttonBoxConfiguration, SIGNAL(accepted()), this, SLOT(enregistrerConfig()));
    connect(ui->buttonBoxConfiguration, SIGNAL(accepted()), parent, SLOT(recuperationParametres()));
    connect(ui->pushButtonAideConfiguration, SIGNAL(clicked()), this, SLOT(aideConfiguration()));
    connect(ui->buttonBoxConfiguration, SIGNAL(clicked(QAbstractButton*)), this, SLOT(fermerAide()));
    connect(ui->comboBoxLongueur, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsConfigurationParite()));
    connect(ui->comboBoxParite, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsConfigurationStop()));
    connect(this, SIGNAL(rejected()), this, SLOT(fermerAide()));
    connect(ui->comboBoxProtocole, SIGNAL(currentIndexChanged(int)), this, SLOT(ASCII_RTU_TCP()));
    connect(ui->pushButtonConfigDefaut, SIGNAL(clicked(bool)), this, SLOT(configurationDefaut()));
    connect(ui->lineEditAdresseIP, SIGNAL(textEdited(QString)), this, SLOT(verificationIP()));
}

void Configuration::chargerConfig()
{
    //Crétion du fichier
    QFile fichier(cheminConfigXML);

    //Si fichier non ouvert -> valeurs par défaut + fermeture méthode
    if(!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Erreur à l'ouverture du fichier", "Le document XML n'a pas pu être ouvert.");
        ui->spinBoxAdresseAfficheur->setValue(1);
        ui->comboBoxProtocole->setCurrentIndex(0);
        ui->comboBoxPortCOM->setCurrentIndex(0);
        ui->comboBoxDebit->setCurrentIndex(3);
        ui->comboBoxLongueur->setCurrentIndex(1);
        ui->comboBoxParite->setCurrentIndex(2);
        ui->comboBoxBitStop->setCurrentIndex(0);
        ui->lineEditAdresseIP->setText("192.168.0.52");
        return;
    }

    //Si fichier non lié au Document -> valeurs par défaut + fermeture méthode
    if (!configXML.setContent(&fichier))
    {
         fichier.close();
         QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Le document XML n'a pas pu être attribué à l'objet QDomDocument.");
         ui->spinBoxAdresseAfficheur->setValue(1);
         ui->comboBoxProtocole->setCurrentIndex(0);
         ui->comboBoxPortCOM->setCurrentIndex(0);
         ui->comboBoxDebit->setCurrentIndex(3);
         ui->comboBoxLongueur->setCurrentIndex(1);
         ui->comboBoxParite->setCurrentIndex(2);
         ui->comboBoxBitStop->setCurrentIndex(0);
         ui->lineEditAdresseIP->setText("192.168.0.52");
         return;
    }
    QDomElement racine = configXML.namedItem("configuration").toElement();

    //Si racine non présente -> valeurs par défaut + fermeture méthode
    if (racine.isNull())
    {
        fichier.close();
        QMessageBox::warning(this, "Erreur lors du parcours du document XML", "<configuration> n'a pas été trouvé, chargement de la configuration par défaut.");
        ui->spinBoxAdresseAfficheur->setValue(1);
        ui->comboBoxProtocole->setCurrentIndex(0);
        ui->comboBoxPortCOM->setCurrentIndex(0);
        ui->comboBoxDebit->setCurrentIndex(3);
        ui->comboBoxLongueur->setCurrentIndex(1);
        ui->comboBoxParite->setCurrentIndex(2);
        ui->comboBoxBitStop->setCurrentIndex(0);
        ui->lineEditAdresseIP->setText("192.168.0.52");
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
                ui->spinBoxAdresseAfficheur->setValue(1);
            }
            else
            {
                ui->spinBoxAdresseAfficheur->setValue(parametres.text().toInt());
            }
        }
        if (parametres.tagName() == "protocole")
        {
            if (parametres.isNull() || parametres.text() == "")
            {
                ui->comboBoxProtocole->setCurrentIndex(0);
            }
            else
            {
                ui->comboBoxProtocole->setCurrentIndex(parametres.text().toInt());
            }
        }
        if (parametres.tagName() == "serie")
        {
            QDomElement paramSerie;
            paramSerie = parametres.firstChildElement("port");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                ui->comboBoxPortCOM->setCurrentIndex(0);
            }
            else
            {
                ui->comboBoxPortCOM->setCurrentIndex(paramSerie.text().toInt());
            }
            paramSerie = parametres.firstChildElement("debit");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                ui->comboBoxDebit->setCurrentIndex(3);
            }
            else
            {
                ui->comboBoxDebit->setCurrentIndex(paramSerie.text().toInt());
            }
            paramSerie = parametres.firstChildElement("longueur");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                ui->comboBoxLongueur->setCurrentIndex(1);
            }
            else
            {
                ui->comboBoxLongueur->setCurrentIndex(paramSerie.text().toInt());
            }
            paramSerie = parametres.firstChildElement("parite");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                ui->comboBoxParite->setCurrentIndex(2);
            }
            else
            {
                ui->comboBoxParite->setCurrentIndex(paramSerie.text().toInt());
            }
            paramSerie = parametres.firstChildElement("stop");
            if (paramSerie.isNull() || paramSerie.text() == "")
            {
                ui->comboBoxBitStop->setCurrentIndex(0);
            }
            else
            {
                ui->comboBoxBitStop->setCurrentIndex(paramSerie.text().toInt());
            }
        }
        if (parametres.tagName() == "ethernet")
        {
            QDomElement paramEthernet;
            paramEthernet = parametres.firstChildElement("IP");
            if (paramEthernet.isNull() || paramEthernet.text() == "")
            {
                ui->lineEditAdresseIP->setText("192.168.0.52");
            }
            else
            {
                ui->lineEditAdresseIP->setText(paramEthernet.text());
            }
        }
    }
    fichier.close();
}

void Configuration::configurationDefaut()
{
    ui->spinBoxAdresseAfficheur->setValue(1);
    ui->comboBoxProtocole->setCurrentIndex(0);
    ui->comboBoxPortCOM->setCurrentIndex(0);
    ui->comboBoxDebit->setCurrentIndex(3);
    ui->comboBoxLongueur->setCurrentIndex(1);
    ui->comboBoxParite->setCurrentIndex(2);
    ui->comboBoxBitStop->setCurrentIndex(0);
    ui->lineEditAdresseIP->setText("192.168.0.52");
}

void Configuration::verificationIP()
{
    QString oIpRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp oIpRegex ("^" + oIpRange
                        + "(\\." + oIpRange + ")?"
                        + "(\\." + oIpRange + ")?"
                        + "(\\." + oIpRange + ")?$");
    ui->lineEditAdresseIP->setValidator(new QRegExpValidator(oIpRegex));
}

void Configuration::enregistrerConfig()
{
    //Création du document
    QDomDocument document;

    //Création des instructions de traitement
    QDomNode instruction = document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"");
    document.insertBefore(instruction, document.firstChild());

    //Création de la racine du document
    QDomElement racine = document.createElement("configuration");
    document.appendChild(racine);

    //Création du premier noeud
    QDomElement adresse = document.createElement("adresse");
    racine.appendChild(adresse);
    QDomText valeurAdresse = document.createTextNode(QString::number(ui->spinBoxAdresseAfficheur->value()));
    adresse.appendChild(valeurAdresse);

    //Création des noeuds suivants...
    QDomElement protocole = document.createElement("protocole");
    racine.appendChild(protocole);
    QDomText valeurProtocole = document.createTextNode(QString::number(ui->comboBoxProtocole->currentIndex()));
    protocole.appendChild(valeurProtocole);

    QDomElement asciiRTU = document.createElement("serie");
    racine.appendChild(asciiRTU);

    //Noeuds inclus dans "serie"
    QDomElement portCom = document.createElement("port");
    asciiRTU.appendChild(portCom);
    QDomText valeurPortCom = document.createTextNode(QString::number(ui->comboBoxPortCOM->currentIndex()));
    portCom.appendChild(valeurPortCom);
    QDomElement debit = document.createElement("debit");
    asciiRTU.appendChild(debit);
    QDomText valeurDebit = document.createTextNode(QString::number(ui->comboBoxDebit->currentIndex()));
    debit.appendChild(valeurDebit);
    QDomElement longueur = document.createElement("longueur");
    asciiRTU.appendChild(longueur);
    QDomText valeurLongueur = document.createTextNode(QString::number(ui->comboBoxLongueur->currentIndex()));
    longueur.appendChild(valeurLongueur);
    QDomElement parite = document.createElement("parite");
    asciiRTU.appendChild(parite);
    QDomText valeurParite = document.createTextNode(QString::number(ui->comboBoxParite->currentIndex()));
    parite.appendChild(valeurParite);
    QDomElement bitStop = document.createElement("stop");
    asciiRTU.appendChild(bitStop);
    QDomText valeurBitStop = document.createTextNode(QString::number(ui->comboBoxBitStop->currentIndex()));
    bitStop.appendChild(valeurBitStop);

    QDomElement TCP = document.createElement("ethernet");
    racine.appendChild(TCP);

    //Noeuds inclus dans "ethernet"
    QDomElement adresseIP = document.createElement("IP");
    TCP.appendChild(adresseIP);
    QDomText valeurAdresseIP = document.createTextNode(ui->lineEditAdresseIP->text());
    adresseIP.appendChild(valeurAdresseIP);

    //Création du fichier
    QFile fichier(cheminConfigXML);
    if(!fichier.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Erreur à l'ouverture du document XML", "Echec de l'ouverture du fichier de configuration en écriture");
        return;
    }
    else
    {
        QTextStream stream(&fichier);
        stream << document.toString();
        fichier.close();
    }
}

void Configuration::aideConfiguration()
{
    //Fermeture de la fenêtre précédente
    aide->close();
    delete aide;

    //Ouverture d'une nouvelle fenêtre
    aide = new Aide(ui->spinBoxAdresseAfficheur->value(),
                       ui->comboBoxProtocole->currentText(),
                       ui->comboBoxDebit->currentText(),
                       ui->comboBoxLongueur->currentText(),
                       ui->comboBoxParite->currentText(),
                       ui->comboBoxBitStop->currentText(),
                       this);
    aide->move(this->x() + this->width() + 20,this->y());
    aide->show();
}

void Configuration::fermerAide()
{
    aide->close();
}

void Configuration::optionsConfigurationParite()
{
    if(ui->comboBoxLongueur->currentIndex() == 0)
    {
        int index = ui->comboBoxParite->currentIndex();
        QStringList parite = {"Paire", "Impaire"};
        ui->comboBoxParite->clear();
        ui->comboBoxParite->insertItems(0, parite);
        if(index < 2)
            ui->comboBoxParite->setCurrentIndex(index);
        else
            ui->comboBoxParite->setCurrentIndex(0);
    }
    else
    {
        int index = ui->comboBoxParite->currentIndex();
        QStringList parite = {"Paire", "Impaire", "Pas de parité"};
        ui->comboBoxParite->clear();
        ui->comboBoxParite->insertItems(0, parite);
        ui->comboBoxParite->setCurrentIndex(index);
    }
    optionsConfigurationStop();
}

void Configuration::optionsConfigurationStop()
{
    if(ui->comboBoxLongueur->currentIndex() == 0 || (ui->comboBoxLongueur->currentIndex() == 1 && ui->comboBoxParite->currentIndex() == 2))
    {
        int index = ui->comboBoxBitStop->currentIndex();
        QStringList stop = {"1 bit", "2 bits"};
        ui->comboBoxBitStop->clear();
        ui->comboBoxBitStop->insertItems(0, stop);
        ui->comboBoxBitStop->setCurrentIndex(index);
    }
    else
    {
        int index = ui->comboBoxBitStop->currentIndex();
        ui->comboBoxBitStop->clear();
        ui->comboBoxBitStop->insertItem(0, "1 bit");
        if(index < 1)
            ui->comboBoxBitStop->setCurrentIndex(index);
        else
            ui->comboBoxBitStop->setCurrentIndex(0);
    }
    rafraichissementAide();
}

void Configuration::rafraichissementAide()
{
    if(aide->isVisible())
    {
        aideConfiguration();
    }
}

void Configuration::ASCII_RTU_TCP()
{
    if(ui->comboBoxProtocole->currentIndex() != 2)
    {
        ui->groupBoxASCII_RTU->setEnabled(true);
        ui->groupBoxTCP->setEnabled(false);
        ui->spinBoxAdresseAfficheur->setEnabled(true);
        if(ui->comboBoxProtocole->currentIndex() == 1)
            ui->spinBoxAdresseAfficheur->setEnabled(false);
    }
    else
    {
        ui->groupBoxASCII_RTU->setEnabled(false);
        ui->groupBoxTCP->setEnabled(true);
        ui->spinBoxAdresseAfficheur->setEnabled(false);
    }
}

Configuration::~Configuration()
{
    delete ui;
}
