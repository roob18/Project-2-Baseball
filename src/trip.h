#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QVector>
#include <QPair>
#include "hashmap.h"
#include "stadiumgraph.h"

struct TripStop {
    QString stadiumName;
    QVector<QPair<QString, int>> purchasedSouvenirs;  // (souvenir name, quantity)
    double totalCost;
};

class Trip {
public:
    Trip();
    
    void addStop(const QString& stadiumName);
    void removeStop(int index);
    void addSouvenir(int stopIndex, const QString& souvenirName, int quantity, double price);
    void sortByStadiumName();
    QVector<TripStop> getStops() const;
    double calculateTotalCost() const;
    double calculateTotalDistance(const StadiumGraph& graph) const;

private:
    QVector<TripStop> stops;
};

#endif // TRIP_H 