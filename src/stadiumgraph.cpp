#include <QSet>
#include <QQueue>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QtGlobal>
#include <algorithm>
#include <queue>
#include <functional>
#include "stadiumgraph.h"
#include "database.h"

StadiumGraph::StadiumGraph() {}

QString StadiumGraph::normalizeStadiumName(const QString& name) {
    if (name.trimmed().isEmpty()) {
        qDebug() << "normalizeStadiumName: Empty or whitespace-only name provided";
        return QString();
    }
    QString n = name.trimmed().toLower();
    // Replace all Unicode dashes with a standard dash
    n.replace(QChar(0x2013), '-'); // en dash
    n.replace(QChar(0x2014), '-'); // em dash
    n.replace(QChar(0x2012), '-'); // figure dash
    n.replace(QChar(0x2015), '-'); // horizontal bar
    // Keep dashes and spaces, but remove other special characters
    n.remove(QRegularExpression("[^a-z0-9\\s-]"));
    // Replace multiple spaces with a single space
    n = n.simplified();
    if (n.isEmpty()) {
        qDebug() << "normalizeStadiumName: Name became empty after normalization:" << name;
        return QString();
    }
    return n;
}

void StadiumGraph::addStadium(const QString& name) {
    QString norm = normalizeStadiumName(name);
    if (norm.isEmpty()) {
        return;
    }
    if (!adjMatrix.contains(norm)) {
        adjMatrix[norm] = QMap<QString, double>();
    }
}

void StadiumGraph::addEdge(const QString& from, const QString& to, double distance) {
    QString nFrom = normalizeStadiumName(from);
    QString nTo = normalizeStadiumName(to);

    // Strict validation of inputs
    if (nFrom.isEmpty() || nTo.isEmpty()) {
        qDebug() << "addEdge: Invalid stadium names - from:" << from << "to:" << to;
        qDebug() << "  Normalized from:" << nFrom << "(hex):" << nFrom.toUtf8().toHex();
        qDebug() << "  Normalized to:" << nTo << "(hex):" << nTo.toUtf8().toHex();
        return;
    }

    if (distance <= 0) {
        qDebug() << "addEdge: Invalid distance:" << distance << "for" << from << "to" << to;
        return;
    }

    // Additional validation to prevent empty keys
    if (nFrom.trimmed().isEmpty() || nTo.trimmed().isEmpty()) {
        qDebug() << "addEdge: Empty or whitespace-only stadium names after normalization";
        qDebug() << "  From:" << nFrom << "(hex):" << nFrom.toUtf8().toHex();
        qDebug() << "  To:" << nTo << "(hex):" << nTo.toUtf8().toHex();
        return;
    }

    // Add stadiums if they don't exist
    if (!adjMatrix.contains(nFrom)) {
        adjMatrix[nFrom] = QMap<QString, double>();
    }
    if (!adjMatrix.contains(nTo)) {
        adjMatrix[nTo] = QMap<QString, double>();
    }

    // Validate the stadiums exist in the matrix
    if (!adjMatrix.contains(nFrom) || !adjMatrix.contains(nTo)) {
        qDebug() << "addEdge: Failed to add stadiums to matrix - from:" << nFrom << "to:" << nTo;
        return;
    }

    // Add the edges
    adjMatrix[nFrom][nTo] = distance;
    adjMatrix[nTo][nFrom] = distance;

    // Verify the edges were added correctly
    if (!adjMatrix[nFrom].contains(nTo) || !adjMatrix[nTo].contains(nFrom)) {
        qDebug() << "addEdge: Failed to add edges - from:" << nFrom << "to:" << nTo;
        return;
    }

    qDebug() << "Added edge:" << nFrom << "<->" << nTo << ":" << distance;
}

double StadiumGraph::getDistance(const QString& from, const QString& to) const {
    QString nFrom = normalizeStadiumName(from);
    QString nTo = normalizeStadiumName(to);
    if (adjMatrix.contains(nFrom) && adjMatrix[nFrom].contains(nTo)) {
        return adjMatrix[nFrom][nTo];
    }
    return -1.0;
}

QVector<QString> StadiumGraph::getStadiums() const {
    return adjMatrix.keys().toVector();
}

QVector<QPair<QString, double>> StadiumGraph::getNeighbors(const QString& stadium) const {
    QVector<QPair<QString, double>> neighbors;
    if (adjMatrix.contains(stadium)) {
        for (auto it = adjMatrix[stadium].begin(); it != adjMatrix[stadium].end(); ++it) {
            neighbors.append(qMakePair(it.key(), it.value()));
        }
    }
    return neighbors;
}

double StadiumGraph::dijkstra(const QString& start, const QString& end, QVector<QString>& path) const {
    try {
        QString nStart = normalizeStadiumName(start);
        QString nEnd = normalizeStadiumName(end);
        
        if (nStart.isEmpty() || nEnd.isEmpty()) {
            path.clear();
        return -1.0;
    }

        if (!adjMatrix.contains(nStart) || !adjMatrix.contains(nEnd)) {
            path.clear();
            return -1.0;
        }

    QMap<QString, double> distances;
    QMap<QString, QString> previous;
    QSet<QString> unvisited;
    
    for (const QString& stadium : adjMatrix.keys()) {
            if (stadium.trimmed().isEmpty()) {
                continue;
            }
        distances[stadium] = std::numeric_limits<double>::infinity();
        unvisited.insert(stadium);
    }
        
        distances[nStart] = 0;

        int iterationCount = 0;
        const int MAX_ITERATIONS = adjMatrix.size() * 2;

        while (!unvisited.isEmpty() && iterationCount < MAX_ITERATIONS) {
            iterationCount++;
            
        QString current;
        double minDist = std::numeric_limits<double>::infinity();
            
        for (const QString& stadium : unvisited) {
                if (!distances.contains(stadium)) {
                    continue;
                }
            if (distances[stadium] < minDist) {
                minDist = distances[stadium];
                current = stadium;
            }
        }

            if (current.isEmpty()) {
                break;
            }
            
            if (current == nEnd) {
                break;
        }

        unvisited.remove(current);

            if (!adjMatrix.contains(current)) {
                break;
            }
            
            const auto& neighbors = adjMatrix[current];
            
            for (auto it = neighbors.begin(); it != neighbors.end(); ++it) {
            const QString& neighbor = it.key();
                
                if (neighbor.trimmed().isEmpty()) {
                    continue;
                }
                
                if (!adjMatrix.contains(neighbor)) {
                    continue;
                }
                
                if (!unvisited.contains(neighbor)) {
                    continue;
                }
                
                if (!distances.contains(neighbor)) {
                    continue;
                }
                
                double edgeWeight = it.value();
                if (edgeWeight <= 0) {
                    continue;
                }
                
                double alt = distances[current] + edgeWeight;
                if (alt < distances[neighbor]) {
                    distances[neighbor] = alt;
                    previous[neighbor] = current;
                }
            }
        }

        path.clear();
        if (!distances.contains(nEnd) || distances[nEnd] == std::numeric_limits<double>::infinity()) {
            return -1.0;
        }
        
        QString current = nEnd;
        QSet<QString> visitedForLoop;
        int pathLength = 0;
        const int MAX_PATH_LENGTH = adjMatrix.size() + 1;
        
        while (current != nStart && pathLength < MAX_PATH_LENGTH) {
            if (visitedForLoop.contains(current)) {
    path.clear();
                return -1.0;
    }

            visitedForLoop.insert(current);
        path.prepend(current);
            pathLength++;
            
            if (!previous.contains(current)) {
                path.clear();
                return -1.0;
            }
            
        current = previous[current];
    }
        
        if (pathLength >= MAX_PATH_LENGTH) {
            path.clear();
            return -1.0;
        }
        
        path.prepend(nStart);
        
        if (path.isEmpty() || path.first() != nStart || path.last() != nEnd) {
            path.clear();
            return -1.0;
        }

        return distances[nEnd];
        
    } catch (...) {
        path.clear();
        return -1.0;
    }
}

double StadiumGraph::aStar(const QString& start, const QString& end, QVector<QString>& path) const {
    if (!adjMatrix.contains(start) || !adjMatrix.contains(end)) {
        return -1.0;
    }

    // Priority queue for open set (f_score, stadium)
    QMap<double, QSet<QString>> openSet;
    QSet<QString> closedSet;
    QMap<QString, double> g_score;  // Cost from start to current
    QMap<QString, double> f_score;  // Estimated total cost
    QMap<QString, QString> came_from;

    // Initialize scores
    for (const QString& stadium : adjMatrix.keys()) {
        g_score[stadium] = std::numeric_limits<double>::infinity();
        f_score[stadium] = std::numeric_limits<double>::infinity();
    }
    g_score[start] = 0;
    f_score[start] = 0;  // For start node, f_score = g_score since h_score = 0
    openSet[0].insert(start);

    while (!openSet.isEmpty()) {
        // Get stadium with lowest f_score
        double current_f = openSet.firstKey();
        QString current = *openSet[current_f].begin();
        openSet[current_f].remove(current);
        if (openSet[current_f].isEmpty()) {
            openSet.remove(current_f);
        }

        if (current == end) {
            // Reconstruct path
            path.clear();
            while (current != start) {
                path.prepend(current);
                current = came_from[current];
            }
            path.prepend(start);
            return g_score[end];
        }

        closedSet.insert(current);

        // Check all neighbors
        for (auto it = adjMatrix[current].begin(); it != adjMatrix[current].end(); ++it) {
            const QString& neighbor = it.key();
            if (closedSet.contains(neighbor)) {
                continue;
            }

            double tentative_g_score = g_score[current] + it.value();

            if (tentative_g_score < g_score[neighbor]) {
                came_from[neighbor] = current;
                g_score[neighbor] = tentative_g_score;
                f_score[neighbor] = g_score[neighbor];  // f_score = g_score since we don't have heuristic

                // Add to open set
                openSet[f_score[neighbor]].insert(neighbor);
            }
        }
    }

    return -1.0;  // No path found
}

double StadiumGraph::minimumSpanningTree(QVector<QPair<QString, QString>>& mstEdges) const {
    mstEdges.clear();
    if (adjMatrix.isEmpty()) {
        qDebug() << "MST: Empty graph";
        return 0.0;
    }

    // Verify graph is connected
    if (!isConnected()) {
        qDebug() << "MST: Graph is not connected";
        return -1.0;
    }

    // Clean up any empty keys or neighbors before starting
    QMap<QString, QMap<QString, double>> cleanAdjMatrix;
    for (auto it = adjMatrix.begin(); it != adjMatrix.end(); ++it) {
        QString stadium = it.key();
        if (stadium.trimmed().isEmpty()) {
            qDebug() << "MST: Skipping empty stadium name";
            continue;
        }
        QMap<QString, double> cleanNeighbors;
        for (auto nIt = it.value().begin(); nIt != it.value().end(); ++nIt) {
            QString neighbor = nIt.key();
            if (neighbor.trimmed().isEmpty()) {
                qDebug() << "MST: Skipping empty neighbor for" << stadium;
                continue;
            }
            if (nIt.value() <= 0) {
                qDebug() << "MST: Skipping invalid distance for" << stadium << "->" << neighbor;
                continue;
            }
            cleanNeighbors[neighbor] = nIt.value();
        }
        if (!cleanNeighbors.isEmpty()) {
            cleanAdjMatrix[stadium] = cleanNeighbors;
        }
    }

    if (cleanAdjMatrix.isEmpty()) {
        qDebug() << "MST: No valid edges after cleaning";
        return -1.0;
    }

    QSet<QString> visited;
    QMap<QString, double> key;
    QMap<QString, QString> parent;
    double totalWeight = 0.0;

    // Initialize keys to infinity
    for (const QString& stadium : cleanAdjMatrix.keys()) {
        key[stadium] = std::numeric_limits<double>::infinity();
    }

    // Start with first stadium
    QString start = cleanAdjMatrix.firstKey();
    key[start] = 0;

    while (visited.size() < cleanAdjMatrix.size()) {
        // Find unvisited vertex with minimum key
        QString current;
        double minKey = std::numeric_limits<double>::infinity();
        
        for (const QString& stadium : cleanAdjMatrix.keys()) {
            if (!visited.contains(stadium) && key[stadium] < minKey) {
                minKey = key[stadium];
                current = stadium;
            }
        }

        if (minKey == std::numeric_limits<double>::infinity() || current.isEmpty()) {
            qDebug() << "MST: No valid unvisited vertex found";
            mstEdges.clear();
            return -1.0;
        }

        visited.insert(current);

        // Add edge to MST if not the first vertex
        if (current != start) {
            if (!parent.contains(current)) {
                qDebug() << "MST: No parent found for" << current;
                continue;
            }
            const QString& parentStadium = parent[current];
            if (!cleanAdjMatrix.contains(parentStadium) || !cleanAdjMatrix.contains(current)) {
                qDebug() << "MST: Parent or current stadium not in clean graph";
                continue;
            }
            mstEdges.append(qMakePair(parentStadium, current));
            totalWeight += key[current];
            qDebug() << "MST added edge:" << parentStadium << "<->" << current 
                     << "weight:" << key[current];
        }

        // Update keys of adjacent vertices
        for (auto it = cleanAdjMatrix[current].begin(); it != cleanAdjMatrix[current].end(); ++it) {
            const QString& neighbor = it.key();
            if (!visited.contains(neighbor) && it.value() < key[neighbor]) {
                parent[neighbor] = current;
                key[neighbor] = it.value();
            }
        }
    }

    // Verify MST has correct number of edges
    if (mstEdges.size() != cleanAdjMatrix.size() - 1) {
        qDebug() << "MST: Incorrect number of edges. Expected:" << (cleanAdjMatrix.size() - 1)
                 << "Got:" << mstEdges.size();
        mstEdges.clear();
        return -1.0;
    }

    return totalWeight;
}

void StadiumGraph::dfsUtil(const QString& current, QSet<QString>& visited, QVector<QString>& order, double& totalDistance, const QString& prev) const {
    visited.insert(current);
    order.append(current);
    if (!prev.isEmpty()) {
        totalDistance += getDistance(prev, current);
    }
    
    // Get all unvisited neighbors with their distances
    QVector<QPair<QString, double>> unvisitedNeighbors;
    for (auto it = adjMatrix[current].begin(); it != adjMatrix[current].end(); ++it) {
        QString neighbor = it.key();
        if (!visited.contains(neighbor)) {
            unvisitedNeighbors.append(qMakePair(neighbor, it.value()));
        }
    }
    
    // Sort neighbors by distance (shortest first)
    std::sort(unvisitedNeighbors.begin(), unvisitedNeighbors.end(),
              [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                  return a.second < b.second;
              });
    
    // Visit neighbors in order of increasing distance
    for (const auto& neighbor : unvisitedNeighbors) {
        dfsUtil(neighbor.first, visited, order, totalDistance, current);
    }
}

double StadiumGraph::dfs(const QString& start, QVector<QString>& order) const {
    order.clear();
    QString nStart = normalizeStadiumName(start);
    if (nStart.isEmpty() || !adjMatrix.contains(nStart)) return -1.0;
    QSet<QString> visited;
    double totalDistance = 0.0;
    dfsUtil(nStart, visited, order, totalDistance, "");
    if (order.size() == adjMatrix.size()) {
        // Debug output: print order and distances
        qDebug() << "DFS Order:";
        for (int i = 0; i < order.size(); ++i) {
            qDebug() << i+1 << ":" << order[i];
        }
        qDebug() << "DFS Step Distances:";
        double runningTotal = 0.0;
        for (int i = 1; i < order.size(); ++i) {
            double dist = getDistance(order[i-1], order[i]);
            runningTotal += dist;
            qDebug() << order[i-1] << "->" << order[i] << ":" << dist << ", running total:" << runningTotal;
                }
        qDebug() << "DFS Total Distance:" << totalDistance;
        return totalDistance;
    } else {
        return -1.0;
    }
}

double StadiumGraph::bfs(const QString& start, QVector<QString>& order) const {
    // Force cleaning before BFS
    const_cast<StadiumGraph*>(this)->cleanAdjacencyMatrix();
    
    order.clear();
    QString nStart = normalizeStadiumName(start);
    if (nStart.isEmpty() || !adjMatrix.contains(nStart)) {
        qDebug() << "BFS: Invalid start stadium:" << start;
        return -1.0;
    }

    QSet<QString> visited;
    QQueue<QString> queue;
    double totalDistance = 0.0;
    visited.insert(nStart);
    queue.enqueue(nStart);
    order.append(nStart);
    int maxIterations = adjMatrix.size() * 10;
    int iterations = 0;

    while (!queue.isEmpty()) {
        if (++iterations > maxIterations) {
            break;
        }
        QString current = queue.dequeue();
        if (!adjMatrix.contains(current)) {
            continue;
        }
        const QMap<QString, double>& currentNeighbors = adjMatrix[current];
        QVector<QPair<QString, double>> unvisitedNeighbors;
        for (auto it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it) {
            QString neighbor = it.key();
            if (neighbor.trimmed().isEmpty()) {
                continue;
            }
            if (!adjMatrix.contains(neighbor)) {
                continue;
            }
            if (visited.contains(neighbor)) {
                continue;
            }
            double distance = it.value();
            if (distance <= 0) {
                continue;
            }
            unvisitedNeighbors.append(qMakePair(neighbor, distance));
        }
        std::sort(unvisitedNeighbors.begin(), unvisitedNeighbors.end(),
                  [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
                      return a.second < b.second;
                  });
        for (const auto& neighbor : unvisitedNeighbors) {
            const QString& nextStadium = neighbor.first;
            double distance = neighbor.second;
            if (nextStadium.trimmed().isEmpty() || !adjMatrix.contains(nextStadium)) {
                continue;
            }
            visited.insert(nextStadium);
            queue.enqueue(nextStadium);
            order.append(nextStadium);
            totalDistance += distance;
        }
    }
    if (order.size() == adjMatrix.size()) {
        return totalDistance;
    } else {
        return -1.0;
    }
}

double StadiumGraph::greedyTrip(const QString& start, const QVector<QString>& stops, QVector<QString>& order) const {
    try {
        // Validate inputs
        QString nStart = normalizeStadiumName(start);
        if (!adjMatrix.contains(nStart)) {
            qDebug() << "Start stadium not found:" << start << "(normalized:" << nStart << ")";
        return -1.0;
    }
        if (stops.isEmpty()) {
            qDebug() << "No stops provided for trip";
            return -1.0;
        }
        // Verify all stops exist in the graph
        QVector<QString> normalizedStops;
        for (const QString& stop : stops) {
            QString nStop = normalizeStadiumName(stop);
            if (!adjMatrix.contains(nStop)) {
                qDebug() << "Stop stadium not found:" << stop << "(normalized:" << nStop << ")";
                continue; // skip missing stadiums
            }
            normalizedStops.append(nStop);
    }
        QSet<QString> unvisited(normalizedStops.begin(), normalizedStops.end());
    order.clear();
        order.append(nStart);
    double totalDistance = 0.0;
        QString current = nStart;
        QVector<QString> skipped;
    while (!unvisited.isEmpty()) {
        // Find nearest unvisited stadium
        QString nearest;
        double minDist = std::numeric_limits<double>::infinity();
        for (const QString& stop : unvisited) {
                try {
            double dist = getDistance(current, stop);
            if (dist >= 0 && dist < minDist) {
                minDist = dist;
                nearest = stop;
                    }
                } catch (...) {
                    continue;
            }
        }
        if (minDist == std::numeric_limits<double>::infinity()) {
                // No reachable stadiums left from current
                // Remove all remaining unvisited stadiums as skipped
                for (const QString& s : unvisited) skipped.append(s);
                break;
        }
        // Move to nearest stadium
        current = nearest;
        unvisited.remove(current);
        order.append(current);
        totalDistance += minDist;
            qDebug() << "Added to trip:" << current << "Distance:" << minDist;
    }
        if (!skipped.isEmpty()) {
            qDebug() << "Skipped unreachable stadiums in greedyTrip:" << skipped;
        }
        qDebug() << "Trip planning complete. Total distance:" << totalDistance;
    return totalDistance;
    } catch (const std::exception& e) {
        qDebug() << "Error in greedyTrip:" << e.what();
        return -1.0;
    } catch (...) {
        qDebug() << "Unknown error in greedyTrip";
        return -1.0;
    }
}

void StadiumGraph::debugPrintAllEdges() const {
    qDebug() << "All edges in StadiumGraph:";
    for (const auto& from : adjMatrix.keys()) {
        for (const auto& to : adjMatrix[from].keys()) {
            if (from < to) { // avoid duplicate undirected edges
                qDebug() << from << "->" << to << ":" << adjMatrix[from][to];
            }
        }
    }
}

void StadiumGraph::debugPrintAllNormalizedStadiums() const {
    qDebug() << "All normalized stadium names in StadiumGraph:";
    for (const QString& stadium : adjMatrix.keys()) {
        qDebug() << stadium;
    }
}

void StadiumGraph::debugPrintAllStadiumConnections() const {
    try {
        qDebug() << "\n=== Stadium Connections ===";
        for (const QString& stadium : adjMatrix.keys()) {
            try {
                QStringList connections;
                if (adjMatrix.contains(stadium)) {
                    for (auto it = adjMatrix[stadium].begin(); it != adjMatrix[stadium].end(); ++it) {
                        connections << QString("%1 (%2)").arg(it.key()).arg(it.value());
                    }
                    qDebug() << stadium << ":" << connections.join(", ");
                }
            } catch (const std::exception& e) {
                qDebug() << "Error processing connections for stadium" << stadium << ":" << e.what();
            } catch (...) {
                qDebug() << "Unknown error processing connections for stadium" << stadium;
            }
        }
    } catch (const std::exception& e) {
        qDebug() << "Error in debugPrintAllStadiumConnections:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error in debugPrintAllStadiumConnections";
    }
}

void StadiumGraph::debugPrintMissingEdges() const {
    qDebug() << "\n=== Missing Edges (distance -1) ===";
    QVector<QString> stadiums = getStadiums();
    int missingCount = 0;
    for (int i = 0; i < stadiums.size(); ++i) {
        for (int j = i + 1; j < stadiums.size(); ++j) {
            double dist = getDistance(stadiums[i], stadiums[j]);
            if (dist == -1.0) {
                qDebug() << stadiums[i] << "<->" << stadiums[j] << ": -1 (missing)";
                ++missingCount;
            }
        }
    }
    if (missingCount == 0) {
        qDebug() << "No missing edges!";
    } else {
        qDebug() << "Total missing edges:" << missingCount;
    }
}

void StadiumGraph::removeEmptyKeysAndNeighbors() {
    // Remove any empty or whitespace-only keys from adjMatrix
    QList<QString> badKeys;
    for (const QString& key : adjMatrix.keys()) {
        if (key.trimmed().isEmpty()) {
            badKeys.append(key);
        }
    }
    for (const QString& key : badKeys) {
        adjMatrix.remove(key);
        qDebug() << "Removed empty or whitespace-only key from adjMatrix!";
    }
    // Remove any empty or whitespace-only neighbors for all stadiums
    for (auto it = adjMatrix.begin(); it != adjMatrix.end(); ++it) {
        QList<QString> badNeighbors;
        for (const QString& nKey : it.value().keys()) {
            if (nKey.trimmed().isEmpty()) {
                badNeighbors.append(nKey);
            }
        }
        for (const QString& nKey : badNeighbors) {
            it.value().remove(nKey);
            qDebug() << "Removed empty or whitespace-only neighbor for" << it.key();
        }
    }
}

void StadiumGraph::debugPrintAllNeighbors() const {
    // Diagnostic: Print all keys for angelstadium at start
    if (adjMatrix.contains("angelstadium")) {
        qDebug() << "DEBUG: All keys for angelstadium at start of debugPrintAllNeighbors:";
        const auto& angelNeighbors = adjMatrix["angelstadium"];
        for (auto it = angelNeighbors.begin(); it != angelNeighbors.end(); ++it) {
            QString key = it.key();
            if (key.trimmed().isEmpty()) {
                qDebug() << "WARNING: Found empty key in angelstadium neighbors!";
                continue;
            }
            qDebug() << "[" << key << "]";
        }
    }
    for (const QString& stadium : adjMatrix.keys()) {
        if (stadium.trimmed().isEmpty()) {
            qDebug() << "WARNING: Found empty or whitespace-only stadium name in adjMatrix keys, skipping.";
            continue;
        }
        QByteArray stadiumHex = stadium.toUtf8().toHex();
        const auto& neighbors = adjMatrix[stadium];
        for (auto nIt = neighbors.begin(); nIt != neighbors.end(); ++nIt) {
            QString neighbor = nIt.key();
            if (neighbor.trimmed().isEmpty()) {
                qDebug() << "WARNING: Found empty or whitespace-only neighbor key for stadium" << stadium << ". Skipping.";
                continue;
            }
        }
    }
}

bool StadiumGraph::validateGraphIntegrity() const {
    bool valid = true;
    for (const QString& stadium : adjMatrix.keys()) {
        QByteArray stadiumHex = stadium.toUtf8().toHex();
        if (stadium.trimmed().isEmpty()) {
            qCritical() << "FATAL: Found empty or whitespace-only stadium name in adjMatrix! Hex:" << stadiumHex;
            valid = false;
        }
        for (const QString& neighbor : adjMatrix[stadium].keys()) {
            QByteArray neighborHex = neighbor.toUtf8().toHex();
            if (neighbor.trimmed().isEmpty()) {
                qCritical() << "FATAL: Found empty or whitespace-only neighbor for" << stadium << "! Hex:" << neighborHex;
                valid = false;
            }
        }
    }
    return valid;
}

void StadiumGraph::rebuildStadiumConnections(const QString& stadium) {
    if (!adjMatrix.contains(stadium)) {
        return;
    }

    QMap<QString, double> validNeighbors;
    const auto& currentNeighbors = adjMatrix[stadium];
    
    // Only keep neighbors that are non-empty and exist in the graph
    for (auto it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it) {
        QString neighbor = it.key();
        double distance = it.value();
        
        // Skip if neighbor is empty or doesn't exist in graph
        if (neighbor.trimmed().isEmpty() || !adjMatrix.contains(neighbor)) {
            qDebug() << "Rebuilding connections for" << stadium << ": Removing invalid neighbor" 
                     << neighbor << "(hex):" << neighbor.toUtf8().toHex();
            continue;
        }
        
        // Skip if distance is invalid
        if (distance <= 0) {
            qDebug() << "Rebuilding connections for" << stadium << ": Removing neighbor with invalid distance" 
                     << neighbor << "distance:" << distance;
            continue;
        }
        
        validNeighbors[neighbor] = distance;
    }
    
    // Replace the stadium's connections with only valid ones
    adjMatrix[stadium] = validNeighbors;
    qDebug() << "Rebuilt connections for" << stadium << ":" << validNeighbors.size() << "valid neighbors";
}

void StadiumGraph::cleanAdjacencyMatrix() {
    qDebug() << "\n=== Starting deep adjacency matrix cleaning ===";
    QList<QString> emptyStadiums;
    int removedNeighbors = 0;
    int removedStadiums = 0;

    // First pass: identify and remove empty stadiums
    for (auto it = adjMatrix.begin(); it != adjMatrix.end(); ++it) {
        QString stadium = it.key();
        if (stadium.trimmed().isEmpty()) {
            emptyStadiums.append(stadium);
            qDebug() << "Found empty stadium name (hex):" << stadium.toUtf8().toHex();
            continue;
        }
    }

    // Remove empty stadiums
    for (const QString& emptyStadium : emptyStadiums) {
        adjMatrix.remove(emptyStadium);
        removedStadiums++;
        qDebug() << "Removed empty stadium:" << emptyStadium;
    }

    // Second pass: deep clean of all neighbors
    QMap<QString, QMap<QString, double>> cleanMatrix;
    for (auto it = adjMatrix.begin(); it != adjMatrix.end(); ++it) {
        QString stadium = it.key();
        QMap<QString, double> cleanNeighbors;
        
        // Validate each neighbor
        for (auto nIt = it.value().begin(); nIt != it.value().end(); ++nIt) {
            QString neighbor = nIt.key();
            double distance = nIt.value();
            
            // Skip if neighbor is empty or doesn't exist in graph
            if (neighbor.trimmed().isEmpty()) {
                qDebug() << "Found empty neighbor for" << stadium << "(hex):" << neighbor.toUtf8().toHex();
                removedNeighbors++;
                continue;
            }
            
            // Skip if neighbor doesn't exist in graph
            if (!adjMatrix.contains(neighbor)) {
                qDebug() << "Found neighbor not in graph:" << neighbor << "for stadium:" << stadium;
                removedNeighbors++;
                continue;
            }
            
            // Skip if distance is invalid
            if (distance <= 0) {
                qDebug() << "Found invalid distance:" << distance << "for" << stadium << "->" << neighbor;
                removedNeighbors++;
                continue;
            }
            
            cleanNeighbors[neighbor] = distance;
        }
        
        // Only add stadium if it has valid neighbors
        if (!cleanNeighbors.isEmpty()) {
            cleanMatrix[stadium] = cleanNeighbors;
        } else {
            qDebug() << "Removing stadium with no valid neighbors:" << stadium;
            removedStadiums++;
        }
    }

    // Replace the entire adjacency matrix with the cleaned version
    adjMatrix = cleanMatrix;

    // Final validation pass
    qDebug() << "\n=== Final validation pass ===";
    for (const QString& stadium : adjMatrix.keys()) {
        qDebug() << "Validating stadium:" << stadium;
        qDebug() << "  Number of neighbors:" << adjMatrix[stadium].size();
        for (const QString& neighbor : adjMatrix[stadium].keys()) {
            if (neighbor.trimmed().isEmpty()) {
                qCritical() << "FATAL: Found empty neighbor after cleaning! Stadium:" << stadium;
                adjMatrix[stadium].remove(neighbor);
                removedNeighbors++;
            }
            qDebug() << "  Neighbor:" << neighbor << "Distance:" << adjMatrix[stadium][neighbor];
        }
    }

    qDebug() << "\n=== Cleaning complete ===";
    qDebug() << "Removed" << removedStadiums << "empty stadiums";
    qDebug() << "Removed" << removedNeighbors << "invalid neighbors";
    qDebug() << "Remaining stadiums:" << adjMatrix.size();
    
    // Print all neighbors for target field specifically
    if (adjMatrix.contains("target field")) {
        qDebug() << "\nNeighbors for target field after cleaning:";
        const auto& neighbors = adjMatrix["target field"];
        for (const QString& neighbor : neighbors.keys()) {
            qDebug() << "  Neighbor:" << neighbor << "Distance:" << neighbors[neighbor];
        }
    }
}

bool StadiumGraph::loadFromCSV(const QString& filename, bool /*clearExisting*/) {
    if (filename.isEmpty()) {
        qDebug() << "loadFromCSV: Empty filename";
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "loadFromCSV: Could not open file:" << filename;
        return false;
    }

    try {
        QTextStream in(&file);
        // Skip header if it exists
        if (!in.atEnd()) {
            QString header = in.readLine();
            if (!header.contains("Distance") && !header.contains("Mileage")) {
                file.seek(0);
                in.seek(0);
            }
        }

        int lineCount = 0;
        int successCount = 0;
        int errorCount = 0;

        while (!in.atEnd()) {
            try {
                lineCount++;
                QString line = in.readLine().trimmed();
                if (line.isEmpty()) {
                    continue;
                }

                QStringList parts = line.split(',');
                if (parts.size() < 3) {
                    qDebug() << "loadFromCSV: Line" << lineCount << "has insufficient parts:" << line;
                    errorCount++;
                    continue;
                }

                QString from = parts[0].trimmed();
                QString to = parts[1].trimmed();
                QString distStr = parts[2].trimmed();

                // Strict validation of inputs
                if (from.isEmpty() || to.isEmpty() || distStr.isEmpty()) {
                    qDebug() << "loadFromCSV: Line" << lineCount << "has empty fields:" << line;
                    errorCount++;
                    continue;
                }

                bool ok = false;
                double distance = distStr.toDouble(&ok);
                if (!ok || distance <= 0) {
                    qDebug() << "loadFromCSV: Line" << lineCount << "has invalid distance:" << distStr;
                    errorCount++;
                    continue;
                }

                QString nFrom = normalizeStadiumName(from);
                QString nTo = normalizeStadiumName(to);

                if (nFrom.isEmpty() || nTo.isEmpty()) {
                    qDebug() << "loadFromCSV: Line" << lineCount << "normalized to empty names:";
                    qDebug() << "  From:" << from << "->" << nFrom << "(hex):" << nFrom.toUtf8().toHex();
                    qDebug() << "  To:" << to << "->" << nTo << "(hex):" << nTo.toUtf8().toHex();
                    errorCount++;
                    continue;
                }

                // Additional validation to prevent empty keys
                if (nFrom.trimmed().isEmpty() || nTo.trimmed().isEmpty()) {
                    qDebug() << "loadFromCSV: Line" << lineCount << "has empty names after normalization:";
                    qDebug() << "  From:" << nFrom << "(hex):" << nFrom.toUtf8().toHex();
                    qDebug() << "  To:" << nTo << "(hex):" << nTo.toUtf8().toHex();
                    errorCount++;
                    continue;
                }

                addEdge(from, to, distance);
                successCount++;

            } catch (...) {
                qDebug() << "loadFromCSV: Exception processing line" << lineCount;
                errorCount++;
                continue;
            }
        }

        file.close();
        qDebug() << "loadFromCSV: Processed" << lineCount << "lines:"
                 << successCount << "successful," << errorCount << "errors";
        return successCount > 0;

    } catch (...) {
        file.close();
        qDebug() << "loadFromCSV: Exception reading file:" << filename;
        return false;
    }
}

bool StadiumGraph::loadMultipleCSVs(const QStringList& filenames) {
    qDebug() << "\n=== Starting to load multiple CSVs ===";
    qDebug() << "Number of files to process:" << filenames.size();
    if (filenames.isEmpty()) {
        qDebug() << "Error: No files provided";
        return false;
    }
    bool allSuccess = true;
    int successfulFiles = 0;
    for (const QString& filename : filenames) {
        qDebug() << "\nProcessing file:" << filename;
        try {
            bool success = loadFromCSV(filename, false); // Always merge, never clear
            if (success) {
                successfulFiles++;
            } else {
                qDebug() << "Failed to load file:" << filename;
                allSuccess = false;
            }
        } catch (...) {
            qDebug() << "Exception while loading file:" << filename;
            allSuccess = false;
        }
    }
    try {
        qDebug() << "\n=== Loaded Data Summary ===";
        qDebug() << "Total stadiums:" << getStadiums().size();
        for (const QString& stadium : getStadiums()) {
            if (adjMatrix.contains(stadium)) {
                qDebug() << stadium << "has" << adjMatrix[stadium].size() << "connections";
                break;  // Just show one stadium as a sample
            }
        }
        debugPrintAllNeighbors();
    } catch (const std::exception& e) {
        qDebug() << "Error printing summary:" << e.what();
    } catch (...) {
        qDebug() << "Unknown error printing summary";
    }
    return successfulFiles > 0;  // Return true if at least one file was loaded successfully
}

void StadiumGraph::clear() {
    adjMatrix.clear();
}

bool StadiumGraph::isConnected() const {
    QVector<QString> stadiums = getStadiums();
    if (stadiums.isEmpty()) return true;
    QSet<QString> visited;
    QQueue<QString> queue;
    queue.enqueue(stadiums[0]);
    visited.insert(stadiums[0]);
    while (!queue.isEmpty()) {
        QString current = queue.dequeue();
        for (const auto& neighbor : adjMatrix[current].keys()) {
            if (!visited.contains(neighbor)) {
                visited.insert(neighbor);
                queue.enqueue(neighbor);
            }
        }
    }
    if (visited.size() == stadiums.size()) {
        qDebug() << "Graph is fully connected!";
    return true;
    } else {
        qDebug() << "Graph is NOT fully connected! Unreachable stadiums:";
        for (const QString& stadium : stadiums) {
            if (!visited.contains(stadium)) {
                qDebug() << "-" << stadium;
            }
        }
        return false;
    }
}

void StadiumGraph::debugPrintUnreachableStadiums() const {
    qDebug() << "\n=== Unreachable Stadiums Check ===";
    QVector<QString> stadiums = getStadiums();
    for (const QString& start : stadiums) {
        QSet<QString> visited;
        QQueue<QString> queue;
        queue.enqueue(start);
        visited.insert(start);
        while (!queue.isEmpty()) {
            QString current = queue.dequeue();
            for (const QString& neighbor : adjMatrix[current].keys()) {
                if (!visited.contains(neighbor)) {
                    visited.insert(neighbor);
                    queue.enqueue(neighbor);
                }
            }
        }
        if (visited.size() != stadiums.size()) {
            qDebug() << "From" << start << ": unreachable stadiums:";
            for (const QString& stadium : stadiums) {
                if (!visited.contains(stadium)) {
                    qDebug() << "-" << stadium;
                }
            }
        }
    }
    qDebug() << "=== All Stadiums Are Reachable ===\n";
}

void StadiumGraph::debugPrintAllMissingPaths() const {
    qDebug() << "\n=== Missing Paths Between Stadiums (no path exists) ===";
    QVector<QString> stadiums = getStadiums();
    int missingCount = 0;
    for (int i = 0; i < stadiums.size(); ++i) {
        for (int j = i + 1; j < stadiums.size(); ++j) {
            QVector<QString> path;
            double dist = dijkstra(stadiums[i], stadiums[j], path);
            if (dist < 0 || path.isEmpty()) {
                qDebug() << stadiums[i] << "<->" << stadiums[j] << ": no path";
                ++missingCount;
            }
        }
    }
    if (missingCount == 0) {
        qDebug() << "No missing paths! The graph is fully connected.";
    } else {
        qDebug() << "Total missing paths:" << missingCount;
    }
    qDebug() << "=== End Missing Paths Check ===\n";
}

bool StadiumGraph::loadFromDatabase(class Database* db) {
    if (!db) return false;
    clear();
    auto distances = db->getAllDistances();
    for (const auto& entry : distances) {
        const QString& from = entry.first;
        const QString& to = entry.second.first;
        double dist = entry.second.second;
        addEdge(from, to, dist);
    }
    return true;
}

double StadiumGraph::tspNearestNeighbor(const QString& start, QVector<QString>& order) const {
    QVector<QString> allStadiums = getStadiums();
    QString nStart = normalizeStadiumName(start);
    QSet<QString> unvisited(allStadiums.begin(), allStadiums.end());
    unvisited.remove(nStart);

    order.clear();
    order.append(nStart);
    QString current = nStart;
    double totalDistance = 0.0;

    while (!unvisited.isEmpty()) {
        double minDist = std::numeric_limits<double>::infinity();
        QString nearest;
        QVector<QString> bestPath;
        for (const QString& candidate : unvisited) {
            QVector<QString> segmentPath;
            double dist = dijkstra(current, candidate, segmentPath);
            if (dist >= 0 && dist < minDist) {
                minDist = dist;
                nearest = candidate;
                bestPath = segmentPath;
            }
        }
        if (nearest.isEmpty() || minDist == std::numeric_limits<double>::infinity()) {
            // No path found to remaining stadiums
            return -1.0;
        }
        // Add the path (skip the first stadium to avoid duplicates)
        for (int j = 1; j < bestPath.size(); ++j) {
            order.append(bestPath[j]);
        }
        totalDistance += minDist;
        current = nearest;
        unvisited.remove(current);
    }
    return totalDistance;
}

// DFS traversal that collects discovery edges in order
void StadiumGraph::dfsRoute(const QString& start, QVector<RouteEdge>& route) const {
    // Clean adjacency matrix before DFS
    const_cast<StadiumGraph*>(this)->cleanAdjacencyMatrix();
    route.clear();
    QString nStart = normalizeStadiumName(start);
    if (nStart.isEmpty() || !adjMatrix.contains(nStart)) {
        qWarning() << "DFS ABORT: Start stadium invalid or not in graph:" << nStart;
        return;
    }
    if (adjMatrix[nStart].isEmpty()) {
        qWarning() << "DFS ABORT: Start stadium has no neighbors:" << nStart;
        return;
    }
    QSet<QString> visited;
    int maxDepth = 100;
    std::function<void(const QString&, int)> dfs = [&](const QString& u, int depth) {
        visited.insert(u);
        if (adjMatrix[u].contains("")) {
            qWarning() << "Removing empty neighbor from" << u << "before DFS loop";
            adjMatrix[u].remove("");
        }
        QMap<QString, double> localNeighbors = adjMatrix[u];
        QVector<QPair<QString, double>> neighbors;
        for (auto it = localNeighbors.begin(); it != localNeighbors.end(); ++it) {
            if (it.key().trimmed().isEmpty() || it.value() <= 0) {
                qWarning() << "Skipping invalid neighbor in DFS:" << it.key() << "Distance:" << it.value();
                continue;
            }
            neighbors.append(qMakePair(it.key(), it.value()));
        }
        std::sort(neighbors.begin(), neighbors.end(), [](const QPair<QString, double>& a, const QPair<QString, double>& b) {
            return a.second < b.second;
        });
        if (depth > maxDepth) {
            qWarning() << "DFS ABORTED: Recursion depth exceeded at node" << u << ", depth:" << depth;
            return;
        }
        for (const auto& neighbor : neighbors) {
            const QString& v = neighbor.first;
            double w = neighbor.second;
            if (!visited.contains(v)) {
                route.append({u, v, w}); // Discovery edge
                dfs(v, depth + 1);
            }
        }
    };
    dfs(nStart, 0);
}

// Function to sum the mileage of the first n-1 discovery edges
// (DFS spanning tree mileage)
double StadiumGraph::dfsDiscoveryMileage(const QString& start) const {
    QVector<RouteEdge> route;
    dfsRoute(start, route);
    double total = 0.0;
    int n = adjMatrix.size();
    for (int i = 0; i < n - 1 && i < route.size(); ++i) {
        total += route[i].miles;
    }
    return total;
}
