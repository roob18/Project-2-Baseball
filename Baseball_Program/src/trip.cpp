#include "trip.h"
#include <algorithm>
#include <QDate>

Trip::Trip() {}

void Trip::addStop(const QString& teamName)
{
    TripStop stop;
    stop.teamName = teamName;
    stops.push_back(stop);
}

void Trip::addSouvenir(const QString& souvenirName, int quantity)
{
    if (TripStop* currentStop = getCurrentStop()) {
        currentStop->purchasedSouvenirs.append(qMakePair(souvenirName, quantity));
    }
}

int Trip::calculateTotalDistance(const HashMap<QString, StadiumInfo>& stadiumMap) const
{
    int totalDistance = 0;
    for (int i = 0; i < stops.size() - 1; ++i) {
        StadiumInfo current, next;
        if (stadiumMap.get(stops[i].teamName, current) && 
            stadiumMap.get(stops[i + 1].teamName, next)) {
            // This is a simplified distance calculation
            // In a real application, you would use actual distances or coordinates
            totalDistance += current.distanceToCenter + next.distanceToCenter;
        }
    }
    return totalDistance;
}

double Trip::calculateTotalCost() const
{
    double total = 0.0;
    for (const auto& stop : stops) {
        total += stop.totalCost;
    }
    return total;
}

void Trip::sortByTeamName()
{
    std::sort(stops.begin(), stops.end(), 
              [](const TripStop& a, const TripStop& b) {
                  return a.teamName < b.teamName;
              });
}

QVector<TripStop> Trip::getTeamsByLeague(const QString& league, const HashMap<QString, StadiumInfo>& stadiumMap) const
{
    QVector<TripStop> result;
    for (const auto& stop : stops) {
        StadiumInfo info;
        if (stadiumMap.get(stop.teamName, info) && info.league == league) {
            result.push_back(stop);
        }
    }
    return result;
}

QVector<TripStop> Trip::getTeamsBySurface(const QString& surface, const HashMap<QString, StadiumInfo>& stadiumMap) const
{
    QVector<TripStop> result;
    for (const auto& stop : stops) {
        StadiumInfo info;
        if (stadiumMap.get(stop.teamName, info) && info.playingSurface == surface) {
            result.push_back(stop);
        }
    }
    return result;
}

QVector<TripStop> Trip::getTeamsByRoofType(const QString& roofType, const HashMap<QString, StadiumInfo>& stadiumMap) const
{
    QVector<TripStop> result;
    for (const auto& stop : stops) {
        StadiumInfo info;
        if (stadiumMap.get(stop.teamName, info) && info.roofType == roofType) {
            result.push_back(stop);
        }
    }
    return result;
}

QVector<TripStop> Trip::getTeamsByDateOpened(int year, bool before, const HashMap<QString, StadiumInfo>& stadiumMap) const
{
    QVector<TripStop> result;
    for (const auto& stop : stops) {
        StadiumInfo info;
        if (stadiumMap.get(stop.teamName, info)) {
            int stadiumYear = QDate::fromString(info.dateOpened, "yyyy-MM-dd").year();
            if ((before && stadiumYear < year) || (!before && stadiumYear > year)) {
                result.push_back(stop);
            }
        }
    }
    return result;
} 