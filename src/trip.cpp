#include "trip.h"
#include <algorithm>
#include <cmath>

Trip::Trip() {}

void Trip::addStop(const QString& stadiumName) {
    TripStop stop;
    stop.stadiumName = stadiumName;
    stop.totalCost = 0.0;
    stops.append(stop);
}

void Trip::removeStop(int index) {
    if (index >= 0 && index < stops.size()) {
        stops.remove(index);
    }
}

void Trip::addSouvenir(int stopIndex, const QString& souvenirName, int quantity, double price) {
    if (stopIndex >= 0 && stopIndex < stops.size()) {
        stops[stopIndex].purchasedSouvenirs.append(qMakePair(souvenirName, quantity));
        stops[stopIndex].totalCost += price * quantity;
    }
}

void Trip::sortByStadiumName() {
    std::sort(stops.begin(), stops.end(),
              [](const TripStop& a, const TripStop& b) {
                  return a.stadiumName < b.stadiumName;
              });
}

QVector<TripStop> Trip::getStops() const {
    return stops;
}

double Trip::calculateTotalCost() const {
    double total = 0.0;
    for (const auto& stop : stops) {
        total += stop.totalCost;
    }
    return total;
}

double Trip::calculateTotalDistance(const StadiumGraph& graph) const {
    double total = 0.0;
    for (int i = 0; i < stops.size() - 1; ++i) {
        const QString& from = stops[i].stadiumName;
        const QString& to = stops[i + 1].stadiumName;
        double dist = graph.getDistance(from, to);
        if (dist >= 0) {
            total += dist;
        }
    }
    return total;
} 