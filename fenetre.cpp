#include "fenetre.h"
#include "ui_fenetre.h"
#include "connexion.h"

#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include "QFont"
#include "QMouseEvent"
#include "QTimer"

using namespace std;

fenetre::fenetre(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fenetre)
{
    ui->setupUi(this);

    setWindowTitle("Gestion Magasin");
    QWidget::setMouseTracking(true); // so it doesn't work only when mouse's buttons are pressed
    qApp->installEventFilter(this);
    ui->tableWidgetDBTable->hide();

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("pepinieristeDB.db"); // set the database that we want to use
    m_db.open(); // boolean (1 or 0) ; 1 if the database has been succesfully opened ; 0 if the database couldn't be open

    QObject::connect(ui->pushButtonAddOrDelete, &QPushButton::clicked, this, &fenetre::buttonClicked);
    QObject::connect(ui->pushButtonValidatePrice, &QPushButton::clicked, this, &fenetre::setPrix);

    connect(timer, &QTimer::timeout, this, &fenetre::timeOut);

    ui->tableWidgetDBTable = new QTableWidget(this);
    ui->tableWidgetDBTable->hide();

    ui->labelSelectPrice->hide();
    ui->lineEditSelectPrice->hide();
    ui->pushButtonValidatePrice->hide();
}

fenetre::~fenetre()
{
    delete ui;
}

void fenetre::showMainWindow()
{
    this->show();
}

void fenetre::selectedFeature(const QString &whatFeature)
{
    if(whatFeature=="Supprimer"){
        ui->labelH1->setText("Gestion stock - Vente plantes");
        ui->labelH2->setText("Supprimer de la base de données");
        ui->labelQuantity->setText("Quantité à supprimer");
        ui->pushButtonAddOrDelete->setText("Supprimer");
    }
    if(whatFeature=="Simuler"){
        ui->labelH1->setText("Gestion stock - Simulation Vente");
        ui->labelH2->setText("Simuler la vante de X plantes");
        ui->labelQuantity->setText("Quantité à simuler");
        ui->labelType->setText("Type de plante à simuler");
        ui->pushButtonAddOrDelete->setText("Supprimer");
    }
    if(whatFeature=="Consulter"){
        ui->labelH1->setText("Gestion stock - Visualition du stock");
        ui->labelH2->hide();
        ui->labelType->hide();
        ui->labelQuantity->hide();
        ui->comboBoxType->hide();
        ui->pushButtonAddOrDelete->hide();
        ui->spinBoxQuantity->hide();

        QSqlQuery showPlantesFromBDD;

        showPlantesFromBDD.prepare("SELECT * FROM plantes"); //SQL Query that we want to execute
        showPlantesFromBDD.exec(); // execute the prepared query

        vector <QString> IDVector;
        vector <QString> Type_plantesVector;
        vector <QString> QuantiteVector;
        vector <QString> prixVector;

        while(showPlantesFromBDD.next()){

            IDVector.push_back(showPlantesFromBDD.value(0).toString());
            Type_plantesVector.push_back(showPlantesFromBDD.value(1).toString());
            QuantiteVector.push_back(showPlantesFromBDD.value(2).toString());
            prixVector.push_back(showPlantesFromBDD.value(3).toString());
        }

        ui->tableWidgetDBTable->setColumnCount(4);
        ui->tableWidgetDBTable->setRowCount(Type_plantesVector.size());

        QTableWidgetItem* headerId = new QTableWidgetItem("Id");
        ui->tableWidgetDBTable->setHorizontalHeaderItem(0,headerId);

        QTableWidgetItem* headePlante = new QTableWidgetItem("Type_plantes");
        ui->tableWidgetDBTable->setHorizontalHeaderItem(1,headePlante);

        QTableWidgetItem* headerQuantite = new QTableWidgetItem("Quantite");
        ui->tableWidgetDBTable->setHorizontalHeaderItem(2,headerQuantite);

        QTableWidgetItem* headerPrix = new QTableWidgetItem("Prix en €");
        ui->tableWidgetDBTable->setHorizontalHeaderItem(3,headerPrix);

        for(unsigned int i=0;i<Type_plantesVector.size();i++){

            QTableWidgetItem *IdCell=new QTableWidgetItem (IDVector[i]);
            ui->tableWidgetDBTable->setItem(i, 0, IdCell);

            QTableWidgetItem *libelleCell=new QTableWidgetItem (Type_plantesVector[i]);
            ui->tableWidgetDBTable->setItem(i, 1, libelleCell);

            QTableWidgetItem *marqueCell=new QTableWidgetItem (QuantiteVector[i]);
            ui->tableWidgetDBTable->setItem(i, 2, marqueCell);

            QTableWidgetItem *prixCell=new QTableWidgetItem (prixVector[i]);
            ui->tableWidgetDBTable->setItem(i, 3, prixCell);
        }

        ui->tableWidgetDBTable->resizeColumnsToContents();
        ui->tableWidgetDBTable->setGeometry(250,100,300,200);
        ui->tableWidgetDBTable->show();
    }
}

void fenetre::buttonClicked()
{

    QString type = ui->comboBoxType->currentText();

    QMessageBox addPriceOrNot;
    addPriceOrNot.setText("Souhaitez-vous également y ajouter un prix par unité ?");
    addPriceOrNot.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    addPriceOrNot.setDefaultButton(QMessageBox::Yes);

    if(ui->pushButtonAddOrDelete->text()=="Ajouter"){ // It means that we are using the "add" feature


        QSqlQuery queryCheckData;
        queryCheckData.prepare("SELECT Type_plantes from plantes where Type_plantes = :Type_plantes");
        queryCheckData.bindValue(":Type_plantes", type );
        queryCheckData.exec();

        int nb = 0;

        while (queryCheckData.next())
            nb++;


        if(nb>0){ //Cheks if Type_plantes already exists or not. >0 means it already exists

            QSqlQuery checkIfPriceExists;
            checkIfPriceExists.prepare("SELECT Prix from plantes where Type_plantes = :Type_plantes"); // Returns the price of a plante, or "" if no price has been set
            checkIfPriceExists.bindValue(":Type_plantes", type );
            checkIfPriceExists.exec(); // execute the prepared query

            QVariant doesPrixExist;

            while(checkIfPriceExists.next()){
                doesPrixExist = checkIfPriceExists.value(0);
            }
            if(doesPrixExist==""){ //Price of this Type_plantes does not exist yet
                double addPriceOrNotValue = addPriceOrNot.exec();

                if(addPriceOrNotValue==4194304){ // when "Cancel" is pressed
                    ui->labelSelectPrice->hide();
                    ui->lineEditSelectPrice->hide();
                    ui->pushButtonValidatePrice->hide();

                    ui->tableWidgetDBTable->hide();
                    ui->labelQuantity->show();
                    ui->labelType->show();
                    ui->comboBoxType->show();
                    ui->spinBoxQuantity->show();
                }

                if(addPriceOrNotValue==65536){ //when "no" is pressed
                    //                    QSqlQuery createType;
                    //                    createType.prepare("INSERT INTO plantes (Type_plantes, Quantite) VALUES (:Type_plantes, :Quantite)"); //Insert
                    //                    createType.bindValue(":Type_plantes", type );
                    //                    createType.bindValue(":Quantite", ui->spinBoxQuantity->value());
                    //                    createType.exec();

                    QSqlQuery queryCheckQuantity;
                    queryCheckQuantity.prepare("SELECT Quantite from plantes where Type_plantes = :Type_plantes");
                    queryCheckQuantity.bindValue(":Type_plantes", type );
                    queryCheckQuantity.exec();
                    while(queryCheckQuantity.next()){
                        int quantite = queryCheckQuantity.value(0).toInt();
                        int newQuantity = quantite + ui ->spinBoxQuantity->value();

                        // qDebug()<< quantite;
                        // qDebug()<< ui ->spinBoxQuantity->value();
                        // qDebug()<< newQuantity;

                        QSqlQuery queryAddQuantity;
                        queryAddQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                        queryAddQuantity.bindValue(":newQuantity", newQuantity );
                        queryAddQuantity.bindValue(":Type_plantes", type );
                        queryAddQuantity.exec(); // execute the prepared query
                    }
                    QMessageBox quantityAddedWithNoPrice;
                    quantityAddedWithNoPrice.setText("La quantité a bien été ajoutée. Aucun prix n'est ajouté à ce type de plantes");
                    quantityAddedWithNoPrice.exec();
                }

                if(addPriceOrNotValue==16384){ // when "yes" is pressed

                    ui->labelSelectPrice->show();
                    ui->lineEditSelectPrice->show();
                    ui->pushButtonValidatePrice->show();

                    ui->tableWidgetDBTable->hide();
                    ui->labelQuantity->hide();
                    ui->labelType->hide();
                    ui->comboBoxType->hide();
                    ui->spinBoxQuantity->hide();
                }
            }
            else{
                QSqlQuery queryCheckQuantity;
                queryCheckQuantity.prepare("SELECT Quantite from plantes where Type_plantes = :Type_plantes");
                queryCheckQuantity.bindValue(":Type_plantes", type );
                queryCheckQuantity.exec(); // execute the prepared query
                while(queryCheckQuantity.next()){
                    int quantite = queryCheckQuantity.value(0).toInt();
                    int newQuantity = quantite + ui ->spinBoxQuantity->value();

                    // qDebug()<< quantite;
                    // qDebug()<< ui ->spinBoxQuantity->value();
                    // qDebug()<< newQuantity;

                    QSqlQuery queryCheckAddQuantity;
                    queryCheckAddQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                    queryCheckAddQuantity.bindValue(":newQuantity", newQuantity );
                    queryCheckAddQuantity.bindValue(":Type_plantes", type );
                    queryCheckAddQuantity.exec(); // execute the prepared query
                }
                QMessageBox quantityAddedPrice;
                quantityAddedPrice.setText("La quantité a bien été ajoutée.");
                quantityAddedPrice.exec();
            }


        }
        else{


            double addPriceOrNotValue = addPriceOrNot.exec();

            if(addPriceOrNotValue==4194304){
                ui->labelSelectPrice->hide();
                ui->lineEditSelectPrice->hide();
                ui->pushButtonValidatePrice->hide();

                ui->tableWidgetDBTable->hide();
                ui->labelQuantity->show();
                ui->labelType->show();
                ui->comboBoxType->show();
                ui->spinBoxQuantity->show();
            }

            if(addPriceOrNotValue==65536){


                QSqlQuery createType;
                createType.prepare("INSERT INTO plantes (Type_plantes, Quantite) VALUES (:Type_plantes, :Quantite)"); //SQL Query that we want to execute
                createType.bindValue(":Type_plantes", type );
                createType.bindValue(":Quantite", ui->spinBoxQuantity->value());
                createType.exec(); // execute the prepared query

                QMessageBox quantityAddedWithType;
                quantityAddedWithType.setText("Ce type de plantes à bien été ajouté, sa quantité également.");
                quantityAddedWithType.exec();
            }

            if(addPriceOrNotValue==16384){

                ui->labelSelectPrice->show();
                ui->lineEditSelectPrice->show();
                ui->pushButtonValidatePrice->show();

                ui->tableWidgetDBTable->hide();
                ui->labelQuantity->hide();
                ui->labelType->hide();
                ui->comboBoxType->hide();
                ui->spinBoxQuantity->hide();
            }
        }
    }
    if(ui->pushButtonAddOrDelete->text()=="Supprimer"){
        QString type = ui->comboBoxType->currentText();

        qDebug()<<type;

        QSqlQuery queryCheckData;
        queryCheckData.prepare("SELECT Type_plantes from plantes where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
        queryCheckData.bindValue(":Type_plantes", type );
        queryCheckData.exec(); // execute the prepared query

        int nb = 0;

        while (queryCheckData.next())
            nb++;

        if(nb>0){
            QSqlQuery queryCheckQuantity;
            queryCheckQuantity.prepare("SELECT Quantite from plantes where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
            queryCheckQuantity.bindValue(":Type_plantes", type );
            queryCheckQuantity.exec(); // execute the prepared query
            while(queryCheckQuantity.next()){
                int quantite = queryCheckQuantity.value(0).toInt();
                int newQuantity = quantite - ui ->spinBoxQuantity->value();

                qDebug()<< quantite;
                qDebug()<< ui ->spinBoxQuantity->value();
                qDebug()<< newQuantity;

                QSqlQuery queryCheckRemoveQuantity;
                queryCheckRemoveQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                queryCheckRemoveQuantity.bindValue(":newQuantity", newQuantity );
                queryCheckRemoveQuantity.bindValue(":Type_plantes", type );
                queryCheckRemoveQuantity.exec(); // execute the prepared query
            }
            QMessageBox quantityRemoved;
            quantityRemoved.setText("La quantité a bien été retirée.");
            quantityRemoved.exec();
        }
        else{
            QMessageBox notInDB;
            notInDB.setText("Impossible de réduire la quantité d'un produit qui n'est pas en stock.");
            notInDB.exec();
        }
    }

    if(ui->pushButtonAddOrDelete->text()=="Simuler"){

    }
}

void fenetre::setPrix()
{
    QString type = ui->comboBoxType->currentText();

    QSqlQuery queryCheckData;
    queryCheckData.prepare("SELECT Type_plantes from plantes where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
    queryCheckData.bindValue(":Type_plantes", type );
    queryCheckData.exec(); // execute the prepared query

    int nb = 0;

    while (queryCheckData.next())
        nb++;

    if(nb>0){

        QSqlQuery queryCheckQuantity;
        queryCheckQuantity.prepare("SELECT Quantite from plantes where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
        queryCheckQuantity.bindValue(":Type_plantes", type );
        queryCheckQuantity.exec(); // execute the prepared query
        while(queryCheckQuantity.next()){
            int quantite = queryCheckQuantity.value(0).toInt();
            int newQuantity = quantite + ui ->spinBoxQuantity->value();

            QSqlQuery createPrice;
            createPrice.prepare("UPDATE plantes SET Quantite = :Quantite , Prix = :Prix where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
            createPrice.bindValue(":Quantite", newQuantity );
            createPrice.bindValue(":Type_plantes", type );
            createPrice.bindValue(":Prix", ui->lineEditSelectPrice->text().toFloat());
            createPrice.exec(); // execute the prepared query
        }

        QMessageBox quantityAddedWithPrice;
        quantityAddedWithPrice.setText("La quantité et le prix ont bien été ajoutés.");
        quantityAddedWithPrice.exec();
    }
    else{
        QSqlQuery createTypeAndPrice;
        createTypeAndPrice.prepare("INSERT INTO plantes (Type_plantes, Quantite, Prix) VALUES (:Type_plantes, :Quantite, :Prix)"); //SQL Query that we want to execute
        createTypeAndPrice.bindValue(":Type_plantes", type );
        createTypeAndPrice.bindValue(":Quantite", ui->spinBoxQuantity->value());
        createTypeAndPrice.bindValue(":Prix", ui->lineEditSelectPrice->text().toFloat());
        createTypeAndPrice.exec(); // execute the prepared query

        QMessageBox quantityAddedWithPriceAndType;
        quantityAddedWithPriceAndType.setText("Ce type de plante a été ajouté, avec son prix. La quantité a également été ajoutée.");
        quantityAddedWithPriceAndType.exec();


    }

    ui->labelSelectPrice->hide();
    ui->lineEditSelectPrice->hide();
    ui->pushButtonValidatePrice->hide();

    ui->tableWidgetDBTable->hide();
    ui->labelQuantity->show();
    ui->labelType->show();
    ui->comboBoxType->show();
    ui->spinBoxQuantity->show();

}



bool fenetre::eventFilter(QObject *obj, QEvent *event) //https://stackoverflow.com/questions/1935021/getting-mousemoveevents-in-qt
{
    if ((event->type() == QEvent::MouseMove or
         event->type() == QEvent::MouseButtonPress or
         event->type() == QEvent::MouseButtonDblClick)
            && isActiveWindow())
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);




        timer->start(300000);
        qDebug()<<QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y());
    }
    return false;
}

void fenetre::timeOut()
{
    if(isActiveWindow())
        emit disconnected();
    this->hide();

}
