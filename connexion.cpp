#include "connexion.h"
#include "ui_connexion.h"
#include "QMessageBox"
#include "QDebug"

connexion::connexion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connexion)
{

    ui->setupUi(this);
    ui->lineEditPassword->clear();
    setWindowTitle("Connexion");
    QObject::connect(ui->pushButtonConnect, &QPushButton::clicked, this, &connexion::checkPassword);
    ui->lineEditPassword->setEchoMode(QLineEdit::Password); // https://stackoverflow.com/questions/5834412/using-qlineedit-for-passwords


}

connexion::~connexion(){
    delete ui;
}

void connexion::checkPassword(){
    QMessageBox wrongPassword;
    wrongPassword.setText("Mauvais mot de passe");
    if((ui->lineEditPassword->text()=="strongPassword")){
        ui->lineEditPassword->clear();
        if(ui->comboBoxFeature->currentIndex()==0)
            emit feature("Add");

        if(ui->comboBoxFeature->currentIndex()==1)
            emit feature("Delete");

        if(ui->comboBoxFeature->currentIndex()==2)
            emit feature("Consult");

        if(ui->comboBoxFeature->currentIndex()==3)
            emit feature("Simulate");

        if(ui->comboBoxFeature->currentIndex()==4)
            emit feature("Update");
        ui->lineEditPassword->clear();
        this->hide();
        ui->lineEditPassword->clear();
        emit connected();
        ui->lineEditPassword->clear();
    }
    else
        wrongPassword.exec();
}

void connexion::showWindowAndMessage(){
    ui->lineEditPassword->clear();
    QMessageBox gotDisconnected;
    gotDisconnected.setText("Vous avez été déconnécté en raison d'une inactivité prolongée, vous pouvez vous reconnecter.");
    gotDisconnected.exec();
    this->show();
}

void connexion::on_lineEditPassword_returnPressed(){
    checkPassword();
}

