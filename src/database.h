#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QPair>
#include "stadiuminfo.h"
#include "hashmap.h"

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool initialize();
    bool createTables();
    void loadStadiumMap();
    void insertInitialData();
    void initializeSouvenirs();
    bool importFromCSV(const QStringList &filenames);
    bool importSingleCSV(const QString &filename);
    bool insertTeam(const QString &teamName, const QString &stadiumName,
                    int capacity, const QString &location, const QString &surface,
                    const QString &league, const QString &dateOpened,
                    int centerField, const QString &typology, const QString &roof);

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

    QVector<QPair<QString, double>> getSouvenirs(const QString &teamName);
    bool addSouvenir(const QString &teamName, const QString &itemName, double price);
    bool updateSouvenirPrice(const QString &teamName, const QString &itemName, double newPrice);
    bool deleteSouvenir(const QString &teamName, const QString &itemName);
    bool updateSouvenirInMap(const QString &teamName, const QString &itemName, double newPrice);

    StadiumInfo getStadiumInfo(const QString &teamName) const;
    QVector<StadiumInfo> getAllStadiums() const;

    bool validateAdmin(const QString &username, const QString &password);

    QSqlDatabase& database() { return db; }
    void reloadStadiumData() { loadStadiumMap(); }

    const HashMap<QString, StadiumInfo>& getStadiumMap() const { return stadiumMap; }

    void refreshStadiumLists();

private:
    QSqlDatabase db;
    HashMap<QString, StadiumInfo> stadiumMap;
};

#endif // DATABASE_H 