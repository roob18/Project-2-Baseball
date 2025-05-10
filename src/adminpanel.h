#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "database.h"
#include "stadiumgraph.h"

namespace Ui {
class AdminPanel;
}

class AdminPanel : public QDialog
{
    Q_OBJECT

public:
    explicit AdminPanel(Database* database, StadiumGraph* stadiumGraph, QWidget *parent = nullptr);
    ~AdminPanel();

signals:
    void dataChanged();  // New signal to notify when data is changed

private slots:
    void on_importStadiumButton_clicked();
    void on_addSouvenirButton_clicked();
    void on_editSouvenirButton_clicked();
    void on_deleteSouvenirButton_clicked();
    void on_teamComboBox_currentIndexChanged(const QString &teamName);
    void on_closeButton_clicked();
    void saveStadiumChanges();
    void on_importDistancesButton_clicked();

private:
    Ui::AdminPanel *ui;
    Database *db;
    StadiumGraph* stadiumGraph;
    void setupUi();
    void loadTeams();
    void loadSouvenirs(const QString &teamName);
    void loadStadiumData();
    void clearSouvenirFields();
    bool validateSouvenirInput();
    QString currentTeam;
};

#endif // ADMINPANEL_H 