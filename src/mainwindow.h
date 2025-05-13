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
#include "stadiumgraph.h"
#include "tripplanner.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setStadiumGraph(StadiumGraph* graph) { stadiumGraph = graph; }
    Database* getDatabase() { return db; }

public slots:
    void refreshData();  // New slot to refresh window data

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
    void viewTeamSouvenirs();
    void on_adminLoginButton_clicked();
    void on_tripPlannerButton_clicked();
    void on_resetDatabaseButton_clicked();

private:
    Ui::MainWindow *ui;
    Database *db;
    StadiumGraph* stadiumGraph = nullptr;
    void setupConnections();
    void clearResults();
    void displayQueryResults(QSqlQuery &query, const QStringList &headers);
    void loadTeams();
};

#endif // MAINWINDOW_H 