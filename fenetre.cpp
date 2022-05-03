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

    QObject::connect(ui->pushButtonAddOrDelete, &QPushButton::clicked, this, &fenetre::buttonClicked);
    QObject::connect(ui->pushButtonValidatePrice, &QPushButton::clicked, this, &fenetre::setPrix);
    connect(timer, &QTimer::timeout, this, &fenetre::timeOut);

    ui->tableWidgetDBTable->hide();
    ui->tableWidgetDBTable = new QTableWidget(this);
    ui->tableWidgetDBTable->hide();
    ui->lineEditSelectPrice->setValidator(new QDoubleValidator(0, 10000, 2, this));
    ui->labelSelectPrice->hide();
    ui->lineEditSelectPrice->hide();
    ui->pushButtonValidatePrice->hide();
    ui->lcdNumberSimulationReseult->hide();
    ui->labelSimulation->hide();
    ui->labelEuro->hide();
    ui->tableWidgetDBTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qApp->setStyleSheet("QPushButton#pushButtonTest { color: #fff }");
    ui->pushButtonTest->setStyleSheet("color: #fff;"
                                      "background-color: #dc3545;"
                                      "border: 2px solid;"
                                      "");



    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("pepinieristeDB.db"); // set the database that we want to use
    m_db.open(); // boolean (1 or 0) ; 1 if the database has been succesfully opened ; 0 if the database couldn't be open
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

    if(whatFeature=="Delete"){
        ui->labelH1->setText("Gestion stock - Vente plantes");
        ui->labelH2->setText("Supprimer de la base de données");
        ui->labelQuantity->setText("Quantité à supprimer");
        ui->pushButtonAddOrDelete->setText("Supprimer");
    }
    if(whatFeature=="Simulate"){
        ui->labelH1->setText("Gestion stock - Simulation Vente");
        ui->labelH2->setText("Simuler la vante de X plantes");
        ui->labelQuantity->setText("Quantité à simuler");
        ui->labelType->setText("Type de plante à simuler");
        ui->pushButtonAddOrDelete->setText("Simuler");
        ui->labelEuro->show();
        ui->labelSimulation->show();
        ui->lcdNumberSimulationReseult->show();
    }
    if(whatFeature=="Consult"){
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
    if(whatFeature=="Update"){
        ui->labelH1->setText("Gestion stock - Modifier");
        ui->labelH2->setText("Modifier le prix d'une plante");
        ui->spinBoxQuantity->hide();
        ui->labelQuantity->hide();
        ui->pushButtonAddOrDelete->setText("Modifier");
        ui->labelSimulation->hide();
        ui->lcdNumberSimulationReseult->hide();
        ui->labelQuantity->hide();
        ui->spinBoxQuantity->hide();
    }
}

void fenetre::buttonClicked()
{
    QString type = ui->comboBoxType->currentText();
    QVariant doesPrixExist;

    int quantite = ui->spinBoxQuantity->value();

    QMessageBox addPriceOrNot;
    addPriceOrNot.setText("Souhaitez-vous également y ajouter un prix par unité ?");
    addPriceOrNot.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    addPriceOrNot.setDefaultButton(QMessageBox::Yes);

    QMessageBox mustSelectAType;
    mustSelectAType.setText("Vous devez selectionner un type de plante.");

    QMessageBox mustSelectAQuantity;
    mustSelectAQuantity.setText("Vous devez selectionner une quantite.");

    QMessageBox quantityAddedWithNoPrice;
    quantityAddedWithNoPrice.setText("La quantité a bien été ajoutée. Aucun prix n'est ajouté à ce type de plantes.");

    QMessageBox quantityAddedPrice;
    quantityAddedPrice.setText("La quantité a bien été ajoutée.");

    QMessageBox quantityAddedWithType;
    quantityAddedWithType.setText("Ce type de plantes à bien été ajouté, sa quantité également.");

    QMessageBox quantityRemoved;
    quantityRemoved.setText("La quantité a bien été retirée.");

    QMessageBox notInDB;
    notInDB.setText("Ce type de plante n'est pas dans la base de données");

    QMessageBox notEnoughQuantityInDB;
    notEnoughQuantityInDB.setText("Vous ne pouvez pas supprimer plus de plantes qu'il n'y en a dans la base de données.");



    QSqlQuery queryCheckData;
    queryCheckData.prepare("SELECT Type_plantes from plantes where Type_plantes = :Type_plantes");
    queryCheckData.bindValue(":Type_plantes", type );

    QSqlQuery queryCheckQuantity;
    queryCheckQuantity.prepare("SELECT Quantite from plantes where Type_plantes = :Type_plantes");
    queryCheckQuantity.bindValue(":Type_plantes", type );

    QSqlQuery createType;
    createType.prepare("INSERT INTO plantes (Type_plantes, Quantite) VALUES (:Type_plantes, :Quantite)"); //SQL Query that we want to execute
    createType.bindValue(":Type_plantes", type );
    createType.bindValue(":Quantite", quantite);

    QSqlQuery queryCheckPrice;
    queryCheckPrice.prepare("SELECT Prix from plantes where Type_plantes = :Type_plantes");
    queryCheckPrice.bindValue(":Type_plantes", type );


    if(ui->pushButtonAddOrDelete->text()=="Ajouter"){ // It means that we are using the "add" feature

        if(ui->comboBoxType->currentIndex()==0)
            mustSelectAType.exec();

        else{
            if(ui->spinBoxQuantity->value()==0)
                mustSelectAQuantity.exec();

            else{
                queryCheckData.exec();

                int nb = 0;

                while (queryCheckData.next())
                    nb++;


                if(nb>0){ //Cheks if Type_plantes already exists or not. >0 means it already exists


                    queryCheckPrice.exec(); // execute the prepared query



                    while(queryCheckPrice.next()){
                        doesPrixExist = queryCheckPrice.value(0);
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
                            queryCheckQuantity.exec();

                            while(queryCheckQuantity.next()){

                                int newQuantity = quantite + ui ->spinBoxQuantity->value();

                                QSqlQuery queryAddQuantity;
                                queryAddQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                                queryAddQuantity.bindValue(":newQuantity", newQuantity );
                                queryAddQuantity.bindValue(":Type_plantes", type );
                                queryAddQuantity.exec(); // execute the prepared query

                            }

                            quantityAddedWithNoPrice.exec();
                        }

                        if(addPriceOrNotValue==16384){ // when "yes" is pressed
                            ui->labelSelectPrice->show();
                            ui->lineEditSelectPrice->show();
                            ui->pushButtonValidatePrice->show();

                            ui->tableWidgetDBTable->hide();
                            ui->pushButtonAddOrDelete->hide();
                            ui->labelQuantity->hide();
                            ui->labelType->hide();
                            ui->comboBoxType->hide();
                            ui->spinBoxQuantity->hide();
                        }
                    }

                    else{
                        queryCheckQuantity.exec();

                        while(queryCheckQuantity.next()){
                            int newQuantity = quantite + queryCheckQuantity.value(0).toInt();

                            QSqlQuery queryAddQuantity;
                            queryAddQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                            queryAddQuantity.bindValue(":newQuantity", newQuantity );
                            queryAddQuantity.bindValue(":Type_plantes", type );
                            queryAddQuantity.exec();
                        }
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
                        createType.exec();
                        quantityAddedWithType.exec();
                    }

                    if(addPriceOrNotValue==16384){
                        ui->labelSelectPrice->show();
                        ui->lineEditSelectPrice->show();
                        ui->pushButtonValidatePrice->show();

                        ui->tableWidgetDBTable->hide();
                        ui->pushButtonAddOrDelete->hide();
                        ui->labelQuantity->hide();
                        ui->labelType->hide();
                        ui->comboBoxType->hide();
                        ui->spinBoxQuantity->hide();
                    }
                }
            }
        }
    }

    if(ui->pushButtonAddOrDelete->text()=="Supprimer"){

        if(ui->comboBoxType->currentIndex()==0)
            mustSelectAType.exec();

        else{

            if(ui->spinBoxQuantity->value()==0)
                mustSelectAQuantity.exec();

            else {
                queryCheckData.exec(); // execute the prepared query
                int nb = 0;

                while (queryCheckData.next())
                    nb++;

                if(nb>0){
                    queryCheckQuantity.exec(); // execute the prepared query

                    while(queryCheckQuantity.next()){
                        int newQuantity = queryCheckQuantity.value(0).toInt() - quantite;

                        QSqlQuery queryRemoveQuantity;
                        queryRemoveQuantity.prepare("UPDATE plantes SET Quantite = :newQuantity where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
                        queryRemoveQuantity.bindValue(":newQuantity", newQuantity );
                        queryRemoveQuantity.bindValue(":Type_plantes", type );
                        queryRemoveQuantity.exec(); // execute the prepared query
                    }
                    quantityRemoved.exec();
                }

                else
                    notInDB.exec();
            }
        }
    }

    if(ui->pushButtonAddOrDelete->text()=="Simuler"){

        if(ui->comboBoxType->currentIndex()==0)
            mustSelectAType.exec();

        else{

            if(ui->spinBoxQuantity->value()==0)
                mustSelectAQuantity.exec();

            else{
                queryCheckData.exec();
                int nb = 0;

                while (queryCheckData.next())
                    nb++;

                if(nb>0){ // If Type_plantes already exists or not. >0 means it already exists
                    int selectedQuantity = ui->spinBoxQuantity->value();
                    float price{0.0};
                    queryCheckQuantity.exec(); // execute the prepared query

                    while(queryCheckQuantity.next())
                        quantite = queryCheckQuantity.value(0).toInt();

                    queryCheckPrice.exec();

                    while(queryCheckPrice.next()){
                        price = (queryCheckPrice.value(0).toFloat())*selectedQuantity;
                        ui->lcdNumberSimulationReseult->display(price);
                    }
                }

                else
                    notInDB.exec();
            }
        }
    }

    if(ui->pushButtonAddOrDelete->text()=="Modifier"){

        queryCheckData.exec();
        int nb = 0;

        while (queryCheckData.next())
            nb++;

        if(nb>0){
            ui->labelSelectPrice->show();
            ui->lineEditSelectPrice->show();
            ui->pushButtonValidatePrice->show();

            ui->tableWidgetDBTable->hide();
            ui->labelQuantity->hide();
            ui->labelType->hide();
            ui->comboBoxType->hide();
            ui->spinBoxQuantity->hide();
            ui->pushButtonAddOrDelete->hide();
        }
        else
            notInDB.exec();
    }

}

void fenetre::setPrix()
{
    int quantite = ui->spinBoxQuantity->value();
    QString type = ui->comboBoxType->currentText();

    QSqlQuery queryCheckData;
    queryCheckData.prepare("SELECT Type_plantes from plantes where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
    queryCheckData.bindValue(":Type_plantes", type );

    QSqlQuery queryCheckQuantity;
    queryCheckQuantity.prepare("SELECT Quantite from plantes where Type_plantes = :Type_plantes"); //SQL Query that we want to execute
    queryCheckQuantity.bindValue(":Type_plantes", type );

    QSqlQuery createPrice;
    createPrice.prepare("UPDATE plantes SET Quantite = :Quantite , Prix = :Prix where Type_plantes = :Type_plantes"); //SQL Query that we want to execute

    QMessageBox quantityAddedWithPrice;
    quantityAddedWithPrice.setText("La quantité et le prix ont bien été ajoutés.");

    float newPrice=ui->lineEditSelectPrice->text().toFloat();

    QSqlQuery createTypeAndPrice;
    createTypeAndPrice.prepare("INSERT INTO plantes (Type_plantes, Quantite, Prix) VALUES (:Type_plantes, :Quantite, :Prix)"); //SQL Query that we want to execute
    createTypeAndPrice.bindValue(":Type_plantes", type );
    createTypeAndPrice.bindValue(":Quantite", quantite);
    createTypeAndPrice.bindValue(":Prix", newPrice);

    QMessageBox quantityAddedWithPriceAndType;
    quantityAddedWithPriceAndType.setText("Ce type de plante a été ajouté, avec son prix. La quantité a également été ajoutée.");



    QSqlQuery queryChangePrice;
    queryChangePrice.prepare("UPDATE plantes SET Prix = :Prix where Type_plantes = :Type_plantes");
    queryChangePrice.bindValue(":Type_plantes", type );
    queryChangePrice.bindValue(":Prix", newPrice);

    QMessageBox priceSuccessfullyChanged;
    priceSuccessfullyChanged.setText("Le prix a correctement été modifié");

    if(ui->pushButtonAddOrDelete->text()=="Ajouter"){
        queryCheckData.exec(); // execute the prepared query
        int nb = 0;

        while (queryCheckData.next())
            nb++;

        if(nb>0){
            queryCheckQuantity.exec(); // execute the prepared query

            while(queryCheckQuantity.next()){
                int newQuantity = quantite + ui ->spinBoxQuantity->value();
                createPrice.bindValue(":Quantite", newQuantity );
                createPrice.bindValue(":Type_plantes", type );
                createPrice.bindValue(":Prix", newPrice);
                createPrice.exec(); // execute the prepared query
            }

            quantityAddedWithPrice.exec();
        }
        else{
            createTypeAndPrice.exec(); // execute the prepared query
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

    if(ui->pushButtonAddOrDelete->text()=="Modifier"){
        queryCheckData.exec();
        int nb = 0;

        while (queryCheckData.next())
            nb++;

        if(nb>0){
            queryChangePrice.exec();
            priceSuccessfullyChanged.exec();
        }
    }

    ui->labelSelectPrice->hide();
    ui->lineEditSelectPrice->hide();
    ui->pushButtonValidatePrice->hide();

    ui->tableWidgetDBTable->hide();
    ui->labelType->show();
    ui->comboBoxType->show();
    ui->pushButtonAddOrDelete->show();
}

bool fenetre::eventFilter(QObject *obj, QEvent *event) //https://stackoverflow.com/questions/1935021/getting-mousemoveevents-in-qt
{
    if ((event->type() == QEvent::MouseMove or
         event->type() == QEvent::MouseButtonPress or
         event->type() == QEvent::MouseButtonDblClick)
            && isActiveWindow()){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        timer->start(300000);
        qDebug()<<QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y());
    }
    return false;
}

void fenetre::timeOut(){
    if(isActiveWindow())
        emit disconnected();
    this->hide();
}

void fenetre::on_lineEditSelectPrice_returnPressed()
{
    setPrix();
}


void fenetre::on_pushButtonTest_clicked()     //https://prograide.com/pregunta/28710/comment-quitter-correctement-un-programme-qt
{
    QMessageBox logout;
    logout.setText("Voulez-vous vraiment quitter cette super application ?");
    logout.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    logout.setDefaultButton(QMessageBox::Cancel);

    if(logout.exec()==16384){
        exit(EXIT_FAILURE);
        QApplication::quit();
        QCoreApplication::quit();
    }
}
