#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QPair>
#include <QVector>
#include "hashmap.h"

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool initialize();
    bool importFromCSV(const QString &filename);

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
    QVector<QPair<QString, double>> getSouvenirs(const QString &teamName);
    bool addSouvenir(const QString &teamName, const QString &itemName, double price);
    bool updateSouvenirPrice(const QString &teamName, const QString &itemName, double newPrice);
    bool deleteSouvenir(const QString &teamName, const QString &itemName);

    // New methods using HashMap
    StadiumInfo getStadiumInfo(const QString &teamName) const;
    QVector<StadiumInfo> getAllStadiums() const;
    bool updateSouvenirInMap(const QString &teamName, const QString &itemName, double newPrice);

    // Add method to access the database object
    QSqlDatabase& database() { return db; }

private:
    QSqlDatabase db;
    bool createTables();
    void insertInitialData();
    void initializeSouvenirs();
    bool insertTeam(const QString &teamName, const QString &stadiumName,
                   int capacity, const QString &location, const QString &surface,
                   const QString &league, const QString &dateOpened,
                   int centerField, const QString &typology, const QString &roof);
    
    // Custom HashMap for storing stadium information
    HashMap<QString, StadiumInfo> stadiumMap;  // Key: team name, Value: stadium info
    void loadStadiumMap();  // Helper method to load data into HashMap
};

#endif // DATABASE_H 