#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>

class Database
{
public:
    Database();
    ~Database();
    
    bool initialize();
    bool importFromCSV(const QString &csvFilePath);
    QSqlQuery getTeamInfo(const QString &teamName);
    QSqlQuery getAllTeamsSortedByTeamName();
    QSqlQuery getAllTeamsSortedByStadiumName();
    QSqlQuery getAmericanLeagueTeams();
    QSqlQuery getNationalLeagueTeams();
    QSqlQuery getTeamsByTypology();
    QSqlQuery getOpenRoofTeams();
    QSqlQuery getTeamsByDateOpened();
    QSqlQuery getTeamsByCapacity();
    QSqlQuery getTeamsWithGreatestCenterField();
    QSqlQuery getTeamsWithSmallestCenterField();
    
private:
    QSqlDatabase db;
    void createTables();
    void insertInitialData();
};

#endif // DATABASE_H 