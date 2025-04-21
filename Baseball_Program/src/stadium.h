#ifndef STADIUM_H
#define STADIUM_H

#include <QString>
#include <QVector>
#include <QPair>

struct Souvenir {
    QString name;
    double price;
    
    Souvenir(const QString& n, double p) : name(n), price(p) {}
};

class Stadium {
public:
    QString teamName;
    QString stadiumName;
    int seatingCapacity;
    QString location;
    QString playingSurface;
    QString league;
    QString dateOpened;
    int distanceToCenter;
    QString ballparkTypology;
    QString roofType;
    QVector<Souvenir> souvenirs;

    Stadium() : seatingCapacity(0), distanceToCenter(0) {}
    
    Stadium(const QString& team, const QString& stadium) 
        : teamName(team), stadiumName(stadium), seatingCapacity(0), distanceToCenter(0) {}
        
    Stadium(const QString& team, const QString& stadium, int capacity,
            const QString& loc, const QString& surface, const QString& lg,
            const QString& date, int distance, const QString& type,
            const QString& roof)
        : teamName(team), stadiumName(stadium), seatingCapacity(capacity),
          location(loc), playingSurface(surface), league(lg),
          dateOpened(date), distanceToCenter(distance),
          ballparkTypology(type), roofType(roof) {}

    void addSouvenir(const QString& name, double price) {
        souvenirs.push_back(Souvenir(name, price));
    }
};

#endif // STADIUM_H 