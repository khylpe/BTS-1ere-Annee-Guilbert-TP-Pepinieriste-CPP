#include "fenetre.h"
#include "ui_fenetre.h"
#include "connexion.h"

#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include "QFont"

using namespace std;

fenetre::fenetre(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fenetre)
{
    ui->setupUi(this);

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("pepinieristeDB.db"); // set the database that we want to use
    m_db.open(); // boolean (1 or 0) ; 1 if the database has been succesfully opened ; 0 if the database couldn't be open

    QObject::connect(ui->pushButtonAddOrDelete, &QPushButton::clicked, this, &fenetre::buttonClicked);

    ui->tableWidgetDBTable = new QTableWidget(this);
    ui->tableWidgetDBTable->hide();
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
    if(ui->pushButtonAddOrDelete->text()=="Ajouter"){
        QString type = ui->comboBoxType->currentText();

        // qDebug()<<type;

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

                qDebug()<< quantite;
                qDebug()<< ui ->spinBoxQuantity->value();
                qDebug()<< newQuantity;
                QSqlQuery queryCheckAddQuantity;
                queryCheckAddQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                queryCheckAddQuantity.bindValue(":newQuantity", newQuantity );
                queryCheckAddQuantity.bindValue(":Type_plantes", type );
                queryCheckAddQuantity.exec(); // execute the prepared query
            }
        }
        else{
            QSqlQuery createType;
            createType.prepare("INSERT INTO plantes (Type_plantes, Quantite) VALUES (:Type_plantes, :Quantite)"); //SQL Query that we want to execute
            createType.bindValue(":Type_plantes", type );
            createType.bindValue(":Quantite", ui->spinBoxQuantity->value());
            createType.exec(); // execute the prepared query
        }
}
    if(ui->pushButtonAddOrDelete->text()=="Supprimer"){
        QString type = ui->comboBoxType->currentText();

        // qDebug()<<type;

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
        }
        else{
            QMessageBox notInDB;
            notInDB.setText("Impossible de réduire la quantité d'un produit qui n'est pas en stock.");
            notInDB.exec();
        }
    }

}


