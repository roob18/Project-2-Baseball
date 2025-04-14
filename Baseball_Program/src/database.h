#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool initialize();
    bool importFromCSV(const QString &csvFilePath);

    // Query methods
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

    // Souvenir methods
    QSqlQuery getSouvenirs(const QString &teamName);
    bool addSouvenir(const QString &teamName, const QString &itemName, double price);
    bool updateSouvenirPrice(int souvenirId, double newPrice);
    bool deleteSouvenir(int souvenirId);

private:
    QSqlDatabase db;
    void createTables();
    void insertInitialData();
    void initializeSouvenirs();
};

#endif // DATABASE_H 