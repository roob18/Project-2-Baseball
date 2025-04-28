#include "trip.h"
#include <algorithm>
#include <QDate>
#include <QDebug>

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

void Trip::initializeGraph(const HashMap<QString, StadiumInfo>& stadiumMap)
{
    // Clear existing data
    vertices.clear();
    adjacencyList.clear();

    // Initialize vertices
    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    for (const auto& entry : entries) {
        Vertex v;
        v.name = entry.first;
        vertices[entry.first] = v;
    }

    // Initialize adjacency list with distances
    for (const auto& entry : entries) {
        const StadiumInfo& stadium = entry.second;
        QVector<Edge> edges;
        
        // Add edges to all other stadiums
        for (const auto& otherEntry : entries) {
            if (otherEntry.first != entry.first) {
                Edge edge;
                edge.destination = otherEntry.first;
                // Calculate distance between stadiums (you'll need to implement this)
                edge.distance = calculateDistance(stadium, otherEntry.second);
                edges.append(edge);
            }
        }
        
        adjacencyList[entry.first] = edges;
    }
}

QString Trip::getMinDistanceVertex() const
{
    QString minVertex;
    double minDistance = std::numeric_limits<double>::infinity();

    for (const auto& vertex : vertices) {
        if (!vertex.visited && vertex.distance < minDistance) {
            minDistance = vertex.distance;
            minVertex = vertex.name;
        }
    }

    return minVertex;
}

void Trip::relax(const QString& u, const QString& v, double weight)
{
    if (vertices[v].distance > vertices[u].distance + weight) {
        vertices[v].distance = vertices[u].distance + weight;
        vertices[v].previous = u;
    }
}

QVector<QString> Trip::getPath(const QString& start, const QString& end) const
{
    QVector<QString> path;
    QString current = end;

    while (current != start) {
        path.prepend(current);
        current = vertices[current].previous;
    }
    path.prepend(start);

    return path;
}

QVector<QString> Trip::findShortestPath(const QString& start, const QVector<QString>& destinations, 
                                      const HashMap<QString, StadiumInfo>& stadiumMap)
{
    initializeGraph(stadiumMap);
    
    // Set start vertex distance to 0
    vertices[start].distance = 0;
    
    // Main Dijkstra's algorithm loop
    for (int i = 0; i < vertices.size(); ++i) {
        QString u = getMinDistanceVertex();
        if (u.isEmpty()) break;
        
        vertices[u].visited = true;
        
        for (const Edge& edge : adjacencyList[u]) {
            if (!vertices[edge.destination].visited) {
                relax(u, edge.destination, edge.distance);
            }
        }
    }
    
    // Find the optimal path through all destinations
    QVector<QString> remainingDestinations = destinations;
    QVector<QString> path;
    QString current = start;
    
    while (!remainingDestinations.isEmpty()) {
        QString nextDestination;
        double minDistance = std::numeric_limits<double>::infinity();
        
        for (const QString& dest : remainingDestinations) {
            if (vertices[dest].distance < minDistance) {
                minDistance = vertices[dest].distance;
                nextDestination = dest;
            }
        }
        
        QVector<QString> segment = getPath(current, nextDestination);
        path.append(segment.mid(1)); // Skip the first element as it's already in the path
        current = nextDestination;
        remainingDestinations.removeOne(nextDestination);
    }
    
    return path;
}

double Trip::getPathDistance(const QVector<QString>& path, const HashMap<QString, StadiumInfo>& stadiumMap) const
{
    double totalDistance = 0.0;
    
    for (int i = 0; i < path.size() - 1; ++i) {
        const StadiumInfo& current = stadiumMap[path[i]];
        const StadiumInfo& next = stadiumMap[path[i + 1]];
        totalDistance += calculateDistance(current, next);
    }
    
    return totalDistance;
} 