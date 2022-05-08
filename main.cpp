#include "fenetre.h"
#include "connexion.h"
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    fenetre w;
    connexion c;
    QObject::connect(&c, SIGNAL(connected()), &w, SLOT(showMainWindow()));
    QObject::connect(&c, SIGNAL(feature(QString)), &w, SLOT(selectedFeature(QString)));
    QObject::connect(&w, SIGNAL(disconnected()), &c, SLOT(showWindowAndMessage()));
    c.show();
    return a.exec();
}
