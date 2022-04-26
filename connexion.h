#ifndef CONNEXION_H
#define CONNEXION_H

#include <QWidget>
#include <fenetre.h>
#include "QString"

namespace Ui {
class connexion;
}

class connexion : public QWidget
{
    Q_OBJECT

public:

    explicit connexion(QWidget *parent = nullptr);
    ~connexion();

private:
    Ui::connexion *ui;

private slots:
    void checkPassword();

signals:
    void connected();
    void feature(const QString& whatFeature);
};

#endif // CONNEXION_H
