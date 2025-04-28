#ifndef TRIP_H
#define TRIP_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QPair>
#include <limits>
#include "hashmap.h"

struct TripStop {
    QString teamName;
    QVector<QPair<QString, int>> purchasedSouvenirs;  // Souvenir name and quantity
    double totalCost;
    
    TripStop() : totalCost(0.0) {}
};

struct Edge {
    QString destination;
    double distance;
};

struct Vertex {
    QString name;
    double distance;
    QString previous;
    bool visited;
    
    Vertex() : distance(std::numeric_limits<double>::infinity()), visited(false) {}
};

class Trip {
public:
    Trip();
    
    // Add a stadium to the trip
    void addStop(const QString& teamName);
    
    // Add souvenir purchase at current stop
    void addSouvenir(const QString& souvenirName, int quantity);
    
    // Calculate total distance of trip
    double calculateTotalDistance(const HashMap<QString, StadiumInfo>& stadiumMap) const;
    
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

    // New methods for shortest path planning
    QVector<QString> findShortestPath(const QString& start, const QVector<QString>& destinations, 
                                    const HashMap<QString, StadiumInfo>& stadiumMap);
    double getPathDistance(const QVector<QString>& path, const HashMap<QString, StadiumInfo>& stadiumMap) const;

private:
    QVector<TripStop> stops;
    QMap<QString, Vertex> vertices;
    QMap<QString, QVector<Edge>> adjacencyList;

    void initializeGraph(const HashMap<QString, StadiumInfo>& stadiumMap);
    QString getMinDistanceVertex() const;
    void relax(const QString& u, const QString& v, double weight);
    QVector<QString> getPath(const QString& start, const QString& end) const;
};

#endif // TRIP_H 