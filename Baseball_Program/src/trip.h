#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QVector>
#include "hashmap.h"

struct TripStop {
    QString teamName;
    QVector<QPair<QString, int>> purchasedSouvenirs;  // Souvenir name and quantity
    double totalCost;
    
    TripStop() : totalCost(0.0) {}
};

class Trip {
public:
    Trip();
    
    // Add a stadium to the trip
    void addStop(const QString& teamName);
    
    // Add souvenir purchase at current stop
    void addSouvenir(const QString& souvenirName, int quantity);
    
    // Calculate total distance of trip
    int calculateTotalDistance(const HashMap<QString, StadiumInfo>& stadiumMap) const;
    
    // Calculate total cost of trip (including souvenirs)
    double calculateTotalCost() const;
    
    // Get all stops in the trip
    const QVector<TripStop>& getStops() const { return stops; }
    
    // Get current stop
    TripStop* getCurrentStop() { return stops.empty() ? nullptr : &stops.back(); }
    
    // Clear the trip
    void clear() { stops.clear(); }
    
    // Sort stops by team name
    void sortByTeamName();
    
    // Get teams in a specific league
    QVector<TripStop> getTeamsByLeague(const QString& league, const HashMap<QString, StadiumInfo>& stadiumMap) const;
    
    // Get teams by surface type
    QVector<TripStop> getTeamsBySurface(const QString& surface, const HashMap<QString, StadiumInfo>& stadiumMap) const;
    
    // Get teams by roof type
    QVector<TripStop> getTeamsByRoofType(const QString& roofType, const HashMap<QString, StadiumInfo>& stadiumMap) const;
    
    // Get teams by date opened (before or after a specific year)
    QVector<TripStop> getTeamsByDateOpened(int year, bool before, const HashMap<QString, StadiumInfo>& stadiumMap) const;

private:
    QVector<TripStop> stops;
};

#endif // TRIP_H 