#include <QSet>
#include <QQueue>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QtGlobal>
#include "stadiumgraph.h"

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
    if (nFrom.isEmpty() || nTo.isEmpty()) {
        qDebug() << "addEdge: Invalid stadium names - from:" << from << "to:" << to;
        return;
    }
    if (distance <= 0) {
        qDebug() << "addEdge: Invalid distance:" << distance << "for" << from << "to" << to;
        return;
    }
    addStadium(nFrom);
    addStadium(nTo);
    if (!adjMatrix.contains(nFrom) || !adjMatrix.contains(nTo)) {
        qDebug() << "addEdge: Stadium not found in graph - from:" << nFrom << "to:" << nTo;
        return;
    }
    adjMatrix[nFrom][nTo] = distance;
    adjMatrix[nTo][nFrom] = distance;
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

double StadiumGraph::dfs(const QString& start, QVector<QString>& order) const {
    const_cast<StadiumGraph*>(this)->cleanAdjacencyMatrix();
    order.clear();
    QString nStart = normalizeStadiumName(start);
    if (nStart.isEmpty()) {
        qDebug() << "DFS: Invalid start stadium name:" << start;
        return -1.0;
    }
    if (!adjMatrix.contains(nStart)) {
        qDebug() << "DFS: Start stadium not found in graph:" << nStart;
        return -1.0;
    }

    QSet<QString> visited;
    double totalDistance = 0.0;
    int recursionDepth = 0;
    const int MAX_DEPTH = adjMatrix.size() * 2;
    bool success = true;

    std::function<void(const QString&)> dfsVisit = [&](const QString& stadium) {
        qDebug() << "DFS lambda entered for stadium:" << stadium;
        if (!success) return;
        if (recursionDepth > MAX_DEPTH) {
            qDebug() << "DFS: Max recursion depth reached at" << stadium;
            success = false;
            return;
        }
        if (stadium.trimmed().isEmpty()) {
            qDebug() << "DFS: Skipping empty stadium name in lambda.";
            return;
        }
        if (!adjMatrix.contains(stadium)) {
            qDebug() << "DFS: Stadium not in graph in lambda:" << stadium;
            return;
        }
        if (visited.contains(stadium)) {
            qDebug() << "DFS: Already visited" << stadium;
            return;
        }
        visited.insert(stadium);
        order.append(stadium);
        recursionDepth++;
        qDebug() << "DFS visiting:" << stadium;
        const QMap<QString, double>& neighbors = adjMatrix[stadium];
        qDebug() << "DFS: Neighbor map size for" << stadium << ":" << neighbors.size();
        qDebug() << "DFS: All neighbor keys for" << stadium << ":" << neighbors.keys();
        int neighborIdx = 0;
        try {
            for (auto nIt = neighbors.begin(); nIt != neighbors.end(); ++nIt, ++neighborIdx) {
                QString neighbor = nIt.key();
                double distance = nIt.value();
                qDebug() << "DFS: Neighbor #" << neighborIdx << "key:" << neighbor << "value:" << distance;
                if (neighbor.trimmed().isEmpty()) {
                    qDebug() << "DFS: Skipping empty neighbor for" << stadium << "at index" << neighborIdx;
                    continue;
                }
                if (!adjMatrix.contains(neighbor)) {
                    qDebug() << "DFS: Neighbor not in graph:" << neighbor << "at index" << neighborIdx;
                    continue;
                }
                if (neighbor == stadium) {
                    qDebug() << "DFS: Skipping self-loop for" << stadium;
                    continue;
                }
                if (visited.contains(neighbor)) {
                    qDebug() << "DFS: Already visited neighbor" << neighbor;
                    continue;
                }
                if (distance <= 0.0) {
                    qDebug() << "DFS: Invalid distance to neighbor" << neighbor << ":" << distance;
                    continue;
                }
                dfsVisit(neighbor);
                if (!success) return;
            }
        } catch (const std::exception& e) {
            qDebug() << "DFS: Exception in neighbor loop for" << stadium << ":" << e.what();
            success = false;
            return;
        } catch (...) {
            qDebug() << "DFS: Unknown exception in neighbor loop for" << stadium;
            success = false;
            return;
        }
        recursionDepth--;
    };

    dfsVisit(nStart);
    qDebug() << "DFS: Total stadiums visited:" << visited.size();
    return success ? static_cast<double>(visited.size()) : -1.0;
}

double StadiumGraph::bfs(const QString& start, QVector<QString>& order) const {
    const_cast<StadiumGraph*>(this)->cleanAdjacencyMatrix();
    order.clear();
    QString nStart = normalizeStadiumName(start);
    if (nStart.isEmpty()) {
        qDebug() << "BFS: Invalid start stadium name:" << start;
        return -1.0;
    }
    if (!adjMatrix.contains(nStart)) {
        qDebug() << "BFS: Start stadium not found in graph:" << nStart;
        return -1.0;
    }

    QSet<QString> visited;
    QQueue<QString> queue;
    double totalDistance = 0.0;
    bool success = true;

    try {
        visited.insert(nStart);
        queue.enqueue(nStart);
        order.append(nStart);
        qDebug() << "BFS: Starting traversal from:" << nStart;

        while (!queue.isEmpty() && success) {
            QString current = queue.dequeue();
            qDebug() << "BFS: Processing stadium:" << current;
            
            if (!adjMatrix.contains(current)) {
                qDebug() << "BFS: Current stadium not in graph:" << current;
                continue;
            }

            const QMap<QString, double>& neighbors = adjMatrix[current];
            qDebug() << "BFS: Neighbor map size for" << current << ":" << neighbors.size();
            qDebug() << "BFS: All neighbor keys for" << current << ":" << neighbors.keys();

            int neighborIdx = 0;
            try {
                for (auto nIt = neighbors.begin(); nIt != neighbors.end(); ++nIt, ++neighborIdx) {
                    QString neighbor = nIt.key();
                    double distance = nIt.value();
                    qDebug() << "BFS: Neighbor #" << neighborIdx << "key:" << neighbor << "value:" << distance;

                    if (neighbor.trimmed().isEmpty()) {
                        qDebug() << "BFS: Skipping empty neighbor for" << current << "at index" << neighborIdx;
                        continue;
                    }
                    if (!adjMatrix.contains(neighbor)) {
                        qDebug() << "BFS: Neighbor not in graph:" << neighbor << "at index" << neighborIdx;
                        continue;
                    }
                    if (neighbor == current) {
                        qDebug() << "BFS: Skipping self-loop for" << current;
                        continue;
                    }
                    if (visited.contains(neighbor)) {
                        qDebug() << "BFS: Already visited neighbor" << neighbor;
                        continue;
                    }
                    if (distance <= 0.0) {
                        qDebug() << "BFS: Invalid distance to neighbor" << neighbor << ":" << distance;
                        continue;
                    }

                    visited.insert(neighbor);
                    queue.enqueue(neighbor);
                    order.append(neighbor);
                    totalDistance += distance;
                    qDebug() << "BFS: Added to queue:" << neighbor << "Distance:" << distance;
                }
            } catch (const std::exception& e) {
                qDebug() << "BFS: Exception in neighbor loop for" << current << ":" << e.what();
                success = false;
                break;
            } catch (...) {
                qDebug() << "BFS: Unknown exception in neighbor loop for" << current;
                success = false;
                break;
            }
        }

        qDebug() << "BFS: Total stadiums visited:" << visited.size();
        if (visited.size() != adjMatrix.size()) {
            qDebug() << "BFS: Not all stadiums were visited. Visited:" << visited.size() 
                     << "Total:" << adjMatrix.size();
            QSet<QString> allStadiums(adjMatrix.keys().begin(), adjMatrix.keys().end());
            QSet<QString> unvisited = allStadiums - visited;
            qDebug() << "BFS: Unvisited stadiums:";
            for (const QString& stadium : unvisited) {
                qDebug() << "-" << stadium;
            }
        }

    } catch (const std::exception& e) {
        qDebug() << "BFS: Exception in main traversal:" << e.what();
        success = false;
    } catch (...) {
        qDebug() << "BFS: Unknown exception in main traversal";
        success = false;
    }

    return success ? totalDistance : -1.0;
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
                return -1.0;
            }
            normalizedStops.append(nStop);
    }

        QSet<QString> unvisited(normalizedStops.begin(), normalizedStops.end());
    order.clear();
        order.append(nStart);
    double totalDistance = 0.0;
        QString current = nStart;

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
                } catch (const std::exception& e) {
                    qDebug() << "Error calculating distance from" << current << "to" << stop << ":" << e.what();
                    continue;
                } catch (...) {
                    qDebug() << "Unknown error calculating distance from" << current << "to" << stop;
                    continue;
            }
        }

        if (minDist == std::numeric_limits<double>::infinity()) {
                qDebug() << "No path found to remaining stops from" << current;
            return -1.0;  // No path to remaining stops
        }

        // Move to nearest stadium
        current = nearest;
        unvisited.remove(current);
        order.append(current);
        totalDistance += minDist;
            
            qDebug() << "Added to trip:" << current << "Distance:" << minDist;
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
    qDebug() << "\n=== All Stadium Neighbors (with hex values) ===";
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
        qDebug() << "Neighbors for" << '"' + stadium + '"' << "(hex:" << stadiumHex << "):";
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

void StadiumGraph::cleanAdjacencyMatrix() {
    QList<QString> emptyStadiums;
    // First pass: identify empty stadiums and collect empty neighbors
    for (auto it = adjMatrix.begin(); it != adjMatrix.end(); ++it) {
        QString stadium = it.key();
        if (stadium.trimmed().isEmpty()) {
            emptyStadiums.append(stadium);
            continue;
        }
        QList<QString> toRemove;
        for (auto nIt = it.value().begin(); nIt != it.value().end(); ++nIt) {
            if (nIt.key().trimmed().isEmpty()) {
                toRemove.append(nIt.key());
            }
        }
        // Remove empty neighbors
        for (const QString& badKey : toRemove) {
            adjMatrix[stadium].remove(badKey);
            qDebug() << "Removed empty neighbor for" << stadium;
        }
    }
    // Second pass: remove empty stadiums
    for (const QString& emptyStadium : emptyStadiums) {
        adjMatrix.remove(emptyStadium);
        qDebug() << "Removed empty stadium:" << emptyStadium;
    }
}

bool StadiumGraph::loadFromCSV(const QString& filename, bool /*clearExisting*/) {
    if (filename.isEmpty()) {
        return false;
    }
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
    while (!in.atEnd()) {
            try {
                lineCount++;
                QString line = in.readLine().trimmed();
                if (line.isEmpty()) {
                    continue;
                }
                QStringList parts = line.split(',');
                if (parts.size() < 3) {
                    continue;
                }
            QString from = parts[0].trimmed();
            QString to = parts[1].trimmed();
                QString distStr = parts[2].trimmed();
                if (from.isEmpty() || to.isEmpty() || distStr.isEmpty()) {
                    continue;
                }
            bool ok = false;
                double distance = distStr.toDouble(&ok);
                if (!ok || distance <= 0) {
                    continue;
                }
                QString nFrom = normalizeStadiumName(from);
                QString nTo = normalizeStadiumName(to);
                if (nFrom.isEmpty() || nTo.isEmpty()) {
                    continue;
                }
                addEdge(from, to, distance);
                successCount++;
            } catch (...) {
                continue;
        }
    }
    file.close();
        return successCount > 0;
    } catch (...) {
        file.close();
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
        qDebug() << "Sample connections:";
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
    qDebug() << "=== End Unreachable Stadiums Check ===\n";
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