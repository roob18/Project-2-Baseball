#ifndef STADIUMINFO_H
#define STADIUMINFO_H

#include <QString>
#include <QVector>
#include <QPair>

struct StadiumInfo {
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
    QVector<QPair<QString, double>> souvenirs;  // List of souvenirs and their prices
};

#endif // STADIUMINFO_H 