#include "connexion.h"
#include "ui_connexion.h"
#include "QMessageBox"
#include "QDebug"

connexion::connexion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connexion)
{
    ui->setupUi(this);
    setWindowTitle("Connexion");
    QObject::connect(ui->pushButtonConnect, &QPushButton::clicked, this, &connexion::checkPassword);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password); // https://stackoverflow.com/questions/5834412/using-qlineedit-for-passwords


}

connexion::~connexion()
{
    delete ui;
}

void connexion::checkPassword()
{

    QMessageBox wrongPassword;
    wrongPassword.setText("Mauvais mot de passe");
        if((ui->lineEditPassword->text()=="strongPassword")){
            if(ui->comboBoxFeature->currentIndex()==0)
                emit feature("Ajouter");

            if(ui->comboBoxFeature->currentIndex()==1)
                emit feature("Supprimer");

            if(ui->comboBoxFeature->currentIndex()==2)
                emit feature("Consulter");

            if(ui->comboBoxFeature->currentIndex()==3)
                emit feature("Simuler");

            this->hide();
            emit connected();
        }
        else
            wrongPassword.exec();

}

void connexion::showWindowAndMessage()
{
    ui->lineEditPassword->clear();


        QMessageBox gotDisconnected;
        gotDisconnected.setText("Vous avez été déconnécté en raison d'une inactivité prolongée, vous pouvez vous reconnecter.");
        gotDisconnected.exec();
         this->show();



}
