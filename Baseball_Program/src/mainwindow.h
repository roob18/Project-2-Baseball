#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include "database.h"

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

private:
    Ui::MainWindow *ui;
    Database *db;
    void setupConnections();
    void clearResults();
    void displayQueryResults(QSqlQuery &query, const QStringList &headers);
};

#endif // MAINWINDOW_H 