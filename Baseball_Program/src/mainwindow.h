#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include "database.h"
#include "souvenirdialog.h"
#include <QPushButton>
#include "hashmap.h"
#include "adminmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void displayTeamInfo();
    void displayAllTeamsByTeamName();
    void displayAllTeamsByStadiumName();
    void displayAmericanLeagueTeams();
    void displayNationalLeagueTeams();
    void displayTeamsByTypology();
    void displayOpenRoofTeams();
    void displayTeamsByDateOpened();
    void displayTeamsByCapacity();
    void displayGreatestCenterField();
    void displaySmallestCenterField();
    void onImportCSV();
    void viewTeamSouvenirs();
    void on_adminLoginButton_clicked();
    void on_addStadiumButton_clicked();
    void on_updateSouvenirPriceButton_clicked();
    void on_addTraditionalSouvenirButton_clicked();
    void on_deleteTraditionalSouvenirButton_clicked();
    void on_shortestTripButton_clicked();

private:
    Ui::MainWindow *ui;
    Database *db;
    AdminManager *adminManager;
    bool isAdminLoggedIn;
    HashMap<QString, StadiumInfo> stadiumMap;
    void setupConnections();
    void clearResults();
    void displayQueryResults(QSqlQuery &query, const QStringList &headers);
    void setupAdminUI();
    void showAdminOnlyUI(bool show);
};

#endif // MAINWINDOW_H 