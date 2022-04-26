#ifndef FENETRE_H
#define FENETRE_H

#include <QWidget>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class fenetre; }
QT_END_NAMESPACE

class fenetre : public QWidget
{
    Q_OBJECT

public:
    fenetre(QWidget *parent = nullptr);
    ~fenetre();

private:
    Ui::fenetre *ui;
    QSqlDatabase m_db;

private slots:
    void showMainWindow();
    void selectedFeature(const QString& whatFeature);
    void buttonClicked();
    void setPrix();
};
#endif // FENETRE_H
