#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDialog>
#include <QString>
#include <QtXml>
#include <QMessageBox>
#include <QTimer>
#include "aide.h"

namespace Ui {
class Configuration;
}

class Configuration : public QDialog
{
    Q_OBJECT

public:
    explicit Configuration(QWidget *parent = 0);
    ~Configuration();

public slots:
    void enregistrerConfig();
    void aideConfiguration();
    void fermerAide();
    void optionsConfigurationParite();
    void optionsConfigurationStop();
    void rafraichissementAide();
    void ASCII_RTU_TCP();
    void configurationDefaut();
    void verificationIP();

private:
    Ui::Configuration *ui;
    QDomDocument configXML;
    QString cheminConfigXML = "config.xml";
    Aide *aide;

    void chargerConfig();
};

#endif // CONFIGURATION_H
