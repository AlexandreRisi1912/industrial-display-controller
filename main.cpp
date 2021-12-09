#include "afficheur.h"
#include <QApplication>

void ouvrirFichier(QString chemin);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Afficheur w;
    w.show();

    return a.exec();
}
