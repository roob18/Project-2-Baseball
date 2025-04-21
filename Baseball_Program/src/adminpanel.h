#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QDialog>
#include <QSqlQuery>
#include "database.h"

namespace Ui {
class AdminPanel;
}

class AdminPanel : public QDialog
{
    Q_OBJECT

public:
    explicit AdminPanel(Database* db, QWidget *parent = nullptr);
    ~AdminPanel();

private slots:
    void on_importStadiumButton_clicked();
    void on_addSouvenirButton_clicked();
    void on_editSouvenirButton_clicked();
    void on_deleteSouvenirButton_clicked();
    void on_teamComboBox_currentIndexChanged(const QString &teamName);
    void on_closeButton_clicked();

private:
    Ui::AdminPanel *ui;
    Database* db;
    void setupUi();
    void loadTeams();
    void loadSouvenirs(const QString &teamName);
};

#endif // ADMINPANEL_H 