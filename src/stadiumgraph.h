#ifndef STADIUMGRAPH_H
#define STADIUMGRAPH_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QPair>
#include <QSet>

class StadiumGraph {
public:
    StadiumGraph();
    void addStadium(const QString& name);
    void addEdge(const QString& from, const QString& to, double distance);
    double getDistance(const QString& from, const QString& to) const;
    QVector<QString> getStadiums() const;
    QVector<QPair<QString, double>> getNeighbors(const QString& stadium) const;
    void clear();

    // Algorithms
    double dijkstra(const QString& start, const QString& end, QVector<QString>& path) const;
    double aStar(const QString& start, const QString& end, QVector<QString>& path) const;
    double minimumSpanningTree(QVector<QPair<QString, QString>>& mstEdges) const;
    double dfs(const QString& start, QVector<QString>& order) const;
    double bfs(const QString& start, QVector<QString>& order) const;
    double greedyTrip(const QString& start, const QVector<QString>& stops, QVector<QString>& order) const;

    // TSP Nearest Neighbor using Dijkstra between all stadiums
    double tspNearestNeighbor(const QString& start, QVector<QString>& order) const;

    bool loadFromCSV(const QString& filename, bool clearExisting = false);
    bool loadMultipleCSVs(const QStringList& filenames);
    void debugPrintAllEdges() const;
    void debugPrintAllNormalizedStadiums() const;
    void debugPrintAllStadiumConnections() const;
    void debugPrintMissingEdges() const;
    void debugPrintAllNeighbors() const;
    void debugPrintUnreachableStadiums() const;
    void debugPrintAllMissingPaths() const;

    bool isConnected() const;

    bool validateGraphIntegrity() const;

    static QString normalizeStadiumName(const QString& name);

    void cleanAdjacencyMatrix();
    void removeEmptyKeysAndNeighbors();

    bool loadFromDatabase(class Database* db);

    void rebuildStadiumConnections(const QString& stadium);

private:
    QMap<QString, QMap<QString, double>> adjMatrix; // adjacency matrix
    void dfsUtil(const QString& current, QSet<QString>& visited, QVector<QString>& order, double& totalDistance, const QString& prev) const;
};

#endif // STADIUMGRAPH_H 