#ifndef FENETRE_H
#define FENETRE_H

#include <QWidget>
#include <QSqlDatabase>
#include "QTimer"

QT_BEGIN_NAMESPACE
namespace Ui { class fenetre; }
QT_END_NAMESPACE

class fenetre : public QWidget
{
    Q_OBJECT

public:
    fenetre(QWidget *parent = nullptr);
    QTimer *timer = new QTimer(this);
    ~fenetre();

private:
    Ui::fenetre *ui;
    QSqlDatabase m_db;

private slots:
    void showMainWindow();
    void selectedFeature(const QString& whatFeature);
    void buttonClicked();
    void setPrix();
    bool eventFilter(QObject *obj, QEvent *event);
    void timeOut();

    void on_lineEditSelectPrice_returnPressed();

    void on_pushButtonTest_clicked();

signals:
    void disconnected();


public slots:


};
#endif // FENETRE_H
