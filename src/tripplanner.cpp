#include "tripplanner.h"
#include "ui_tripplanner.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <algorithm>
#include <limits>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QProgressDialog>

TripPlanner::TripPlanner(const HashMap<QString, StadiumInfo>& stadiumMap, StadiumGraph* stadiumGraph, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TripPlanner)
    , stadiumMap(stadiumMap)
    , stadiumGraph(stadiumGraph)
{
    ui->setupUi(this);
    setWindowTitle("Trip Planner");
    refreshStadiumLists();
    ui->tripStadiumsList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tripStadiumsList, &QListWidget::itemSelectionChanged, this, &TripPlanner::updateSouvenirTableForSelectedStadium);
    connect(ui->removeStopButton, &QPushButton::clicked, this, &TripPlanner::on_removeStopButton_clicked);
    connect(ui->startingStadiumCombo, &QComboBox::currentTextChanged, this, &TripPlanner::on_startingStadiumCombo_currentIndexChanged);
    if (ui->planTripButton) connect(ui->planTripButton, &QPushButton::clicked, this, &TripPlanner::on_planTripButton_clicked);
    if (ui->algorithmCombo) connect(ui->algorithmCombo, &QComboBox::currentTextChanged, this, &TripPlanner::updateAlgorithmUIVisibility);
    if (ui->removeSouvenirButton) connect(ui->removeSouvenirButton, &QPushButton::clicked, this, &TripPlanner::on_removeSouvenirButton_clicked);
    updateAlgorithmUIVisibility();

    // After loading stadiums and before planning a trip, print debug info
    if (stadiumGraph) {
        stadiumGraph->debugPrintAllNormalizedStadiums();
        stadiumGraph->debugPrintAllEdges();
        stadiumGraph->debugPrintMissingEdges();
        stadiumGraph->debugPrintUnreachableStadiums();
    }
}

TripPlanner::~TripPlanner() {
    delete ui;
}

void TripPlanner::on_dijkstraButton_clicked()
{
    // Use the first stadium in tripStadiumsList as start, and prompt for end
    if (ui->tripStadiumsList->count() == 0) {
        QMessageBox::warning(this, "Error", "Please add at least one stadium to your trip.");
        return;
    }
    QString startTeam = ui->tripStadiumsList->item(0)->text();
    StadiumInfo startInfo;
    if (!stadiumMap.get(startTeam, startInfo)) {
        QMessageBox::warning(this, "Error", QString("Could not find stadium for team '%1'").arg(startTeam));
        return;
    }
    QString startStadium = startInfo.stadiumName.trimmed();

    // Get all user-friendly stadium names for the dropdown
    QVector<QString> stadiums;
    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    for (const auto& entry : entries) {
        stadiums.append(entry.second.stadiumName.trimmed());
    }
    bool ok = false;
    QString endStadium = QInputDialog::getItem(this, "Select Destination", "Choose destination stadium:", stadiums, 0, false, &ok);
    if (!ok || startStadium.isEmpty() || endStadium.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select both start and end stadiums");
        return;
    }
    // Normalize for lookup
    QString nStart = StadiumGraph::normalizeStadiumName(startStadium);
    QString nEnd = StadiumGraph::normalizeStadiumName(endStadium);
    QVector<QString> normalizedStadiums;
    for (const QString& s : stadiums) normalizedStadiums << StadiumGraph::normalizeStadiumName(s);
    if (!normalizedStadiums.contains(nStart)) {
        QMessageBox::warning(this, "Error", QString("Start stadium '%1' not found in graph.").arg(startStadium));
        return;
    }
    if (!normalizedStadiums.contains(nEnd)) {
        QMessageBox::warning(this, "Error", QString("End stadium '%1' not found in graph.").arg(endStadium));
        return;
    }
    // Defensive: Check that both stadiums exist in the graph adjacency matrix
    if (!stadiumGraph->getStadiums().contains(StadiumGraph::normalizeStadiumName(startStadium))) {
        QMessageBox::warning(this, "Error", QString("Start stadium '%1' not found in graph adjacency matrix.").arg(startStadium));
        return;
    }
    if (!stadiumGraph->getStadiums().contains(StadiumGraph::normalizeStadiumName(endStadium))) {
        QMessageBox::warning(this, "Error", QString("End stadium '%1' not found in graph adjacency matrix.").arg(endStadium));
        return;
    }
    // Print all stadiums in the graph before running Dijkstra
    qDebug() << "All stadiums in graph (adjMatrix keys):" << stadiumGraph->getStadiums();
    qDebug() << "Dijkstra start team:" << startTeam << ", stadium:" << startStadium << ", normalized:" << nStart;
    qDebug() << "Dijkstra end stadium:" << endStadium << ", normalized:" << nEnd;
    QVector<QString> path;
    double distance = stadiumGraph->dijkstra(startStadium, endStadium, path);
    qDebug() << "Dijkstra result distance:" << distance << ", path:" << path;
    // Defensive: Check for empty/null/invalid path
    if (distance < 0 || path.isEmpty()) {
        QMessageBox::warning(this, "Trip Error", "No path found between the selected stadiums.");
        ui->tripSummaryText->setText("No path found between the selected stadiums.");
        ui->totalDistanceLabel->setText("Total Distance: 0 miles");
        return;
    }
    for (const QString& stadium : path) {
        if (stadium.isEmpty()) {
            QMessageBox::warning(this, "Trip Error", "Path contains an empty stadium name. Data may be corrupt.");
            ui->tripSummaryText->setText("Path contains an empty stadium name. Data may be corrupt.");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
    }
    QString summary = "Shortest Path (Dijkstra):\n";
    for (const QString& stadium : path) {
        summary += stadium + " -> ";
    }
    if (!path.isEmpty()) summary.chop(4);
    summary += QString("\n\nTotal Distance: %1 miles").arg(distance, 0, 'f', 2);
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(distance, 0, 'f', 2));
    // Update Trip Stadiums list
    ui->tripStadiumsList->clear();
    for (const QString& stadium : path) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}

void TripPlanner::on_mstButton_clicked()
{
    QVector<QPair<QString, QString>> mst;
    double totalWeight = stadiumGraph->minimumSpanningTree(mst);
    QString summary = "Minimum Spanning Tree:\n";
    int validEdgeCount = 0;
    QSet<QString> stadiumsInMST;
    for (const auto& edge : mst) {
        if (edge.first.isNull() || edge.second.isNull() || edge.first.isEmpty() || edge.second.isEmpty()) {
            continue; // skip invalid edges
        }
        summary += edge.first + " -- " + edge.second + "\n";
        ++validEdgeCount;
        stadiumsInMST.insert(edge.first);
        stadiumsInMST.insert(edge.second);
    }
    summary += QString("\nTotal Distance: %1 miles").arg(totalWeight, 0, 'f', 2);
    if (validEdgeCount == 0) {
        QMessageBox::warning(this, "MST Error", "No valid minimum spanning tree could be constructed. The graph may be disconnected or contain invalid data.");
        ui->tripSummaryText->setText("No valid minimum spanning tree could be constructed.");
        ui->totalDistanceLabel->setText("Total Distance: 0 miles");
        return;
    }
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(totalWeight, 0, 'f', 2));
    // Update Trip Stadiums list (all unique stadiums in MST)
    ui->tripStadiumsList->clear();
    for (const QString& stadium : stadiumsInMST) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}

void TripPlanner::on_dfsButton_clicked()
{
    // Use selected team from dfsBfsStartCombo, map to stadium name
    QString teamName;
    if (ui->dfsBfsStartCombo && ui->dfsBfsStartCombo->currentText().trimmed().size() > 0) {
        teamName = ui->dfsBfsStartCombo->currentText().trimmed();
    } else {
        teamName = "San Francisco Giants";
    }
    StadiumInfo info;
    QString startStadium;
    if (stadiumMap.get(teamName, info)) {
        startStadium = info.stadiumName.trimmed();
    } else {
        startStadium = teamName;
    }
    QString normalizedStart = StadiumGraph::normalizeStadiumName(startStadium);
    qDebug() << "All stadiums in graph:" << stadiumGraph->getStadiums();
    qDebug() << "Trying to start DFS at normalized stadium:" << normalizedStart;
    if (!stadiumGraph->getStadiums().contains(normalizedStart)) {
        QMessageBox::warning(this, "Error", teamName + " not found in the graph (normalized: " + normalizedStart + ").");
        return;
    }
    // Use greedy nearest neighbor for DFS as per assignment expectation
    QVector<QString> allStadiums = stadiumGraph->getStadiums();
    QVector<QString> stops;
    for (const QString& s : allStadiums) {
        if (s != normalizedStart) stops.append(s);
    }
    QVector<QString> order;
    double distance = stadiumGraph->greedyTrip(normalizedStart, stops, order);
    if (distance < 0 || order.isEmpty()) {
        QMessageBox::warning(this, "Trip Error", "No path found from " + startStadium + ".");
        ui->tripSummaryText->setText("No path found from " + startStadium + ".");
        ui->totalDistanceLabel->setText("Total Distance: 0 miles");
        return;
    }
    if (order.size() < stadiumGraph->getStadiums().size()) {
        QMessageBox::warning(this, "Trip Warning", "Not all stadiums are reachable from " + startStadium + ".");
    }
    QString summary = "DFS Traversal (" + startStadium + "):\n";
    for (const QString& stadium : order) {
        if (stadium.isNull() || stadium.isEmpty()) {
            QMessageBox::warning(this, "Trip Error", "Path contains an empty or invalid stadium name. Data may be corrupt.");
            ui->tripSummaryText->setText("Path contains an empty or invalid stadium name. Data may be corrupt.");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
        summary += stadium + " -> ";
    }
    if (!order.isEmpty()) summary.chop(4);
    summary += QString("\nTotal Distance: %1 miles").arg(distance, 0, 'f', 2);
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(distance, 0, 'f', 2));
    // Update Trip Stadiums list
    ui->tripStadiumsList->clear();
    for (const QString& stadium : order) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}

void TripPlanner::on_bfsButton_clicked()
{
    // Use selected team from dfsBfsStartCombo, map to stadium name
    QString teamName;
    if (ui->dfsBfsStartCombo && ui->dfsBfsStartCombo->currentText().trimmed().size() > 0) {
        teamName = ui->dfsBfsStartCombo->currentText().trimmed();
    } else {
        teamName = "Minnesota Twins";
    }
    StadiumInfo info;
    QString startStadium;
    if (stadiumMap.get(teamName, info)) {
        startStadium = info.stadiumName.trimmed();
    } else {
        startStadium = teamName;
    }
    QString normalizedStart = StadiumGraph::normalizeStadiumName(startStadium);
    qDebug() << "All stadiums in graph:" << stadiumGraph->getStadiums();
    qDebug() << "Trying to start BFS at normalized stadium:" << normalizedStart;
    if (!stadiumGraph->getStadiums().contains(normalizedStart)) {
        QMessageBox::warning(this, "Error", teamName + " not found in the graph (normalized: " + normalizedStart + ").");
        return;
    }
    QVector<QString> path;
    double distance = stadiumGraph->bfs(normalizedStart, path);
    if (distance < 0 || path.isEmpty()) {
        QMessageBox::warning(this, "Trip Error", "No path found from " + startStadium + ".");
        ui->tripSummaryText->setText("No path found from " + startStadium + ".");
        ui->totalDistanceLabel->setText("Total Distance: 0 miles");
        return;
    }
    if (path.size() < stadiumGraph->getStadiums().size()) {
        QMessageBox::warning(this, "Trip Warning", "Not all stadiums are reachable from " + startStadium + ".");
    }
    QString summary = "BFS Traversal (" + startStadium + "):\n";
    for (const QString& stadium : path) {
        if (stadium.isNull() || stadium.isEmpty()) {
            QMessageBox::warning(this, "Trip Error", "Path contains an empty or invalid stadium name. Data may be corrupt.");
            ui->tripSummaryText->setText("Path contains an empty or invalid stadium name. Data may be corrupt.");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
        summary += stadium + " -> ";
    }
    if (!path.isEmpty()) summary.chop(4);
    summary += QString("\nTotal Distance: %1 miles").arg(distance, 0, 'f', 2);
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(distance, 0, 'f', 2));
    // Update Trip Stadiums list
    ui->tripStadiumsList->clear();
    for (const QString& stadium : path) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}

void TripPlanner::on_filterLeagueCombo_currentIndexChanged(const QString &league) {
    // TODO: Implement league filtering
}

void TripPlanner::updateTripInfo() {
    // TODO: Implement trip info updates
}

void TripPlanner::on_addStopButton_clicked() {
    QList<QListWidgetItem*> selected = ui->availableStadiumsList->selectedItems();
    if (selected.size() > 0) {
        QString stadium = selected.first()->text();
        // Don't add duplicate stadiums
        bool exists = false;
        for (int i = 0; i < ui->tripStadiumsList->count(); ++i) {
            if (ui->tripStadiumsList->item(i)->text() == stadium) {
                exists = true; break;
            }
        }
        if (!exists) {
            ui->tripStadiumsList->addItem(stadium);
        }
        updateSouvenirTableForSelectedStadium();
    }
}

void TripPlanner::on_removeStopButton_clicked() {
    QList<QListWidgetItem*> selected = ui->tripStadiumsList->selectedItems();
    for (QListWidgetItem* item : selected) {
        delete ui->tripStadiumsList->takeItem(ui->tripStadiumsList->row(item));
    }
}

void TripPlanner::on_addSouvenirButton_clicked() {
    // Store souvenirs for the current stadium, merging with existing
    QString stadium;
    if (ui->tripStadiumsList->selectedItems().size() > 0)
        stadium = ui->tripStadiumsList->selectedItems().first()->text();
    else if (ui->tripStadiumsList->count() > 0)
        stadium = ui->tripStadiumsList->item(0)->text();
    else
        stadium = "";
    if (stadium.isEmpty()) return;
    QMap<QString, int> merged;
    // Start with existing purchases
    for (const auto& pair : souvenirCart[stadium]) {
        merged[pair.first] = pair.second;
    }
    // Add new purchases (overwrite with new quantity)
    for (int i = 0; i < ui->souvenirTable->rowCount(); ++i) {
        QString name = ui->souvenirTable->item(i, 0)->text();
        int qty = ui->souvenirTable->item(i, 2)->text().toInt();
        merged[name] = qty;
    }
    // Remove items with 0 quantity
    QVector<QPair<QString, int>> mergedList;
    for (auto it = merged.begin(); it != merged.end(); ++it) {
        if (it.value() > 0) mergedList.append(qMakePair(it.key(), it.value()));
    }
    if (mergedList.isEmpty()) {
        souvenirCart.remove(stadium);
    } else {
        souvenirCart[stadium] = mergedList;
    }
    updateOverallSouvenirSummary();
    QMessageBox::information(this, "Success", "Souvenir(s) added successfully!");
}

void TripPlanner::on_clearTripButton_clicked() {
    ui->tripSummaryText->clear();
    ui->totalDistanceLabel->setText("Total Distance: 0 miles");
    ui->totalCostLabel->setText("Total Cost: $0.00");
}

void TripPlanner::on_sortButton_clicked() {
    // TODO: Implement sorting
}

void TripPlanner::on_aStarButton_clicked() {
    // TODO: Implement A* shortest path logic
}

void TripPlanner::on_greedyButton_clicked() {
    // Start at Marlins Park (Miami Marlins), map and normalize all stadiums
    QString startTeam = "Miami Marlins";
    StadiumInfo startInfo;
    if (!stadiumMap.get(startTeam, startInfo)) {
        QMessageBox::warning(this, "Error", "Could not find Marlins Park (Miami Marlins) in the stadium map.");
        return;
    }
    QString startStadium = StadiumGraph::normalizeStadiumName(startInfo.stadiumName.trimmed());
    // Get all stadiums except the starting one, map and normalize
    QVector<QString> allStadiums;
    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    for (const auto& entry : entries) {
        QString stadium = StadiumGraph::normalizeStadiumName(entry.second.stadiumName.trimmed());
        if (stadium != startStadium) {
            allStadiums.append(stadium);
        }
    }
    qDebug() << "Greedy trip start:" << startStadium << ", stops:" << allStadiums;
    QVector<QString> tripOrder;
    double totalDistance = 0.0;
    QString current = startStadium;
    QVector<QString> unvisited = allStadiums;
    tripOrder.append(current);
    while (!unvisited.isEmpty()) {
        double bestDist = std::numeric_limits<double>::infinity();
        int bestIdx = -1;
        QVector<QString> bestPath;
        for (int i = 0; i < unvisited.size(); ++i) {
            QVector<QString> path;
            double dist = stadiumGraph->dijkstra(current, unvisited[i], path);
            if (dist >= 0 && dist < bestDist) {
                bestDist = dist;
                bestIdx = i;
                bestPath = path;
            }
        }
        if (bestIdx == -1) {
            QMessageBox::warning(this, "Trip Error", QString("No path found to remaining stops from '%1'.").arg(current));
            ui->tripSummaryText->setText("No path found to remaining stops from " + current + ".");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
        // Add the path (skip the first stadium to avoid duplicates)
        for (int j = 1; j < bestPath.size(); ++j) {
            tripOrder.append(bestPath[j]);
        }
        totalDistance += bestDist;
        current = unvisited.takeAt(bestIdx);
    }
    QString summary = "Visit All (Marlins Park, Shortest Paths):\n";
    for (int i = 0; i < tripOrder.size(); ++i) {
        summary += tripOrder[i];
        if (i < tripOrder.size() - 1) summary += " -> ";
    }
    summary += QString("\n\nTotal Distance: %1 miles").arg(totalDistance, 0, 'f', 2);
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(totalDistance, 0, 'f', 2));
    // Update Trip Stadiums list
    ui->tripStadiumsList->clear();
    for (const QString& stadium : tripOrder) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}

void TripPlanner::updateSouvenirTableForSelectedStadium() {
    // Use the selected stadium in tripStadiumsList
    QString selectedTeam;
    if (ui->tripStadiumsList->selectedItems().size() > 0)
        selectedTeam = ui->tripStadiumsList->selectedItems().first()->text();
    else if (ui->tripStadiumsList->count() > 0)
        selectedTeam = ui->tripStadiumsList->item(0)->text();
    else
        selectedTeam = "";
    StadiumInfo info;
    if (!selectedTeam.isEmpty() && stadiumMap.get(selectedTeam, info)) {
        ui->souvenirTable->setRowCount(info.souvenirs.size());
        for (int i = 0; i < info.souvenirs.size(); ++i) {
            const auto& souvenir = info.souvenirs[i];
            QTableWidgetItem* nameItem = new QTableWidgetItem(souvenir.first);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            ui->souvenirTable->setItem(i, 0, nameItem);
            QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(souvenir.second, 'f', 2));
            priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable);
            ui->souvenirTable->setItem(i, 1, priceItem);
            QTableWidgetItem* qtyItem = new QTableWidgetItem("0");
            qtyItem->setFlags(qtyItem->flags() | Qt::ItemIsEditable);
            ui->souvenirTable->setItem(i, 2, qtyItem);
        }
    } else {
        ui->souvenirTable->setRowCount(0);
    }
}

void TripPlanner::on_dodgerToAnyButton_clicked() {
    // TODO: Implement Dodger Stadium to any team trip logic
}
void TripPlanner::on_customOrderTripButton_clicked() {
    // Get stadiums in the order shown in tripStadiumsList, map team name to stadium name, normalize
    QVector<QString> stadiums;
    for (int i = 0; i < ui->tripStadiumsList->count(); ++i) {
        QString teamOrStadium = ui->tripStadiumsList->item(i)->text().trimmed();
        StadiumInfo info;
        QString stadiumName;
        if (stadiumMap.get(teamOrStadium, info)) {
            stadiumName = info.stadiumName.trimmed();
        } else {
            stadiumName = teamOrStadium;
        }
        stadiums.append(StadiumGraph::normalizeStadiumName(stadiumName));
    }
    qDebug() << "Custom Order Trip normalized stadiums:" << stadiums;
    if (stadiums.size() < 2) {
        QMessageBox::warning(this, "Trip Error", "Please select at least two stadiums for a custom order trip.");
        return;
    }
    double totalDistance = 0.0;
    QVector<QString> fullPath;
    for (int i = 0; i < stadiums.size() - 1; ++i) {
        QString from = stadiums[i];
        QString to = stadiums[i + 1];
        QVector<QString> segmentPath;
        double dist = stadiumGraph->dijkstra(from, to, segmentPath);
        qDebug() << "Dijkstra from" << from << "to" << to << ": path=" << segmentPath << ", dist=" << dist;
        if (dist < 0 || segmentPath.isEmpty()) {
            QMessageBox::warning(this, "Trip Error", QString("No path between '%1' and '%2'.").arg(from, to));
            ui->tripSummaryText->setText("No path between selected stadiums.");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
        // Avoid duplicating the starting stadium of each segment (except the first)
        if (i == 0) {
            fullPath += segmentPath;
        } else {
            for (int j = 1; j < segmentPath.size(); ++j) {
                fullPath.append(segmentPath[j]);
            }
        }
        totalDistance += dist;
    }
    QString summary = "Custom Order Trip (Shortest Paths):\n";
    for (int i = 0; i < fullPath.size(); ++i) {
        summary += fullPath[i];
        if (i < fullPath.size() - 1) summary += " -> ";
    }
    summary += QString("\n\nTotal Distance: %1 miles").arg(totalDistance, 0, 'f', 2);
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(totalDistance, 0, 'f', 2));
    // Update Trip Stadiums list
    ui->tripStadiumsList->clear();
    for (const QString& stadium : fullPath) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}
void TripPlanner::on_visitAllMarlinsButton_clicked() {
    // TODO: Implement visit all teams from Marlins Park logic
}
void TripPlanner::on_dreamVacationButton_clicked() {
    QVector<QString> stadiums;
    for (int i = 0; i < ui->tripStadiumsList->count(); ++i) {
        QString teamOrStadium = ui->tripStadiumsList->item(i)->text().trimmed();
        StadiumInfo info;
        QString stadiumName;
        if (stadiumMap.get(teamOrStadium, info)) {
            stadiumName = info.stadiumName.trimmed();
        } else {
            stadiumName = teamOrStadium;
        }
        stadiums.append(StadiumGraph::normalizeStadiumName(stadiumName));
    }
    qDebug() << "Dream Vacation normalized stadiums:" << stadiums;
    if (stadiums.size() < 2) {
        QMessageBox::warning(this, "Trip Error", "Please select at least two stadiums for a dream vacation trip.");
        return;
    }
    if (stadiums.size() <= 8) {
        // Brute-force in a background thread
        QProgressDialog* progress = new QProgressDialog("Finding optimal trip order...", QString(), 0, 0, this);
        progress->setWindowModality(Qt::ApplicationModal);
        progress->setCancelButton(nullptr);
        progress->setMinimumDuration(0);
        progress->show();
        QFuture<QPair<QVector<QString>, double>> future = QtConcurrent::run([=]() {
            QVector<QString> bestOrder;
            double minDistance = std::numeric_limits<double>::infinity();
            QVector<QString> perm = stadiums;
            std::sort(perm.begin(), perm.end());
            do {
                double dist = 0.0;
                bool valid = true;
                for (int i = 0; i < perm.size() - 1; ++i) {
                    QVector<QString> segmentPath;
                    double d = stadiumGraph->dijkstra(perm[i], perm[i+1], segmentPath);
                    if (d < 0 || segmentPath.isEmpty()) { valid = false; break; }
                    dist += d;
                }
                if (valid && dist < minDistance) {
                    minDistance = dist;
                    bestOrder = perm;
                }
            } while (std::next_permutation(perm.begin(), perm.end()));
            return QPair<QVector<QString>, double>(bestOrder, minDistance);
        });
        QFutureWatcher<QPair<QVector<QString>, double>>* watcher = new QFutureWatcher<QPair<QVector<QString>, double>>(this);
        connect(watcher, &QFutureWatcher<QPair<QVector<QString>, double>>::finished, this, [=]() {
            progress->close();
            QPair<QVector<QString>, double> result = watcher->result();
            QVector<QString> bestOrder = result.first;
            double minDistance = result.second;
            if (bestOrder.isEmpty() || minDistance == std::numeric_limits<double>::infinity()) {
                QMessageBox::warning(this, "Trip Error", "Could not find a valid optimized trip order.");
                ui->tripSummaryText->setText("Could not find a valid optimized trip order.");
                ui->totalDistanceLabel->setText("Total Distance: 0 miles");
                watcher->deleteLater();
                progress->deleteLater();
                return;
            }
            // Now, build the full path using Dijkstra for each consecutive pair in bestOrder
            double totalDistance = 0.0;
            QVector<QString> fullPath;
            for (int i = 0; i < bestOrder.size() - 1; ++i) {
                QVector<QString> segmentPath;
                double dist = stadiumGraph->dijkstra(bestOrder[i], bestOrder[i+1], segmentPath);
                if (dist < 0 || segmentPath.isEmpty()) {
                    QMessageBox::warning(this, "Trip Error", QString("No path between '%1' and '%2'.").arg(bestOrder[i], bestOrder[i+1]));
                    ui->tripSummaryText->setText("No path between selected stadiums.");
                    ui->totalDistanceLabel->setText("Total Distance: 0 miles");
                    watcher->deleteLater();
                    progress->deleteLater();
                    return;
                }
                if (i == 0) {
                    fullPath += segmentPath;
                } else {
                    for (int j = 1; j < segmentPath.size(); ++j) {
                        fullPath.append(segmentPath[j]);
                    }
                }
                totalDistance += dist;
            }
            QString summary = "Dream Vacation (Optimized Order, Shortest Paths):\n";
            for (int i = 0; i < fullPath.size(); ++i) {
                summary += fullPath[i];
                if (i < fullPath.size() - 1) summary += " -> ";
            }
            summary += QString("\n\nTotal Distance: %1 miles").arg(totalDistance, 0, 'f', 2);
            ui->tripSummaryText->setText(summary);
            ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(totalDistance, 0, 'f', 2));
            // Update Trip Stadiums list
            ui->tripStadiumsList->clear();
            for (const QString& stadium : fullPath) {
                ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
            }
            if (ui->tripStadiumsList->count() > 0) {
                ui->tripStadiumsList->setCurrentRow(0);
                updateSouvenirTableForSelectedStadium();
            }
            watcher->deleteLater();
            progress->deleteLater();
        });
        watcher->setFuture(future);
        return;
    }
    // ... existing nearest neighbor heuristic code for >8 stadiums ...
    QVector<QString> toVisit = stadiums;
    QVector<QString> order;
    QString current = toVisit.takeFirst();
    order.append(current);
    while (!toVisit.isEmpty()) {
        double bestDist = std::numeric_limits<double>::infinity();
        int bestIdx = -1;
        QVector<QString> bestPath;
        for (int i = 0; i < toVisit.size(); ++i) {
            QVector<QString> segmentPath;
            double d = stadiumGraph->dijkstra(current, toVisit[i], segmentPath);
            if (d >= 0 && d < bestDist) {
                bestDist = d;
                bestIdx = i;
                bestPath = segmentPath;
            }
        }
        if (bestIdx == -1) {
            QMessageBox::warning(this, "Trip Error", "No path between some stadiums in the selection.");
            ui->tripSummaryText->setText("No path between some stadiums in the selection.");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
        current = toVisit.takeAt(bestIdx);
        order.append(current);
    }
    QVector<QString> bestOrder = order;
    double minDistance = 0.0;
    for (int i = 0; i < bestOrder.size() - 1; ++i) {
        QVector<QString> segmentPath;
        double d = stadiumGraph->dijkstra(bestOrder[i], bestOrder[i+1], segmentPath);
        if (d < 0 || segmentPath.isEmpty()) {
            minDistance = std::numeric_limits<double>::infinity();
            break;
        }
        minDistance += d;
    }
    if (bestOrder.isEmpty() || minDistance == std::numeric_limits<double>::infinity()) {
        QMessageBox::warning(this, "Trip Error", "Could not find a valid optimized trip order.");
        ui->tripSummaryText->setText("Could not find a valid optimized trip order.");
        ui->totalDistanceLabel->setText("Total Distance: 0 miles");
        return;
    }
    // Now, build the full path using Dijkstra for each consecutive pair in bestOrder
    double totalDistance = 0.0;
    QVector<QString> fullPath;
    for (int i = 0; i < bestOrder.size() - 1; ++i) {
        QVector<QString> segmentPath;
        double dist = stadiumGraph->dijkstra(bestOrder[i], bestOrder[i+1], segmentPath);
        if (dist < 0 || segmentPath.isEmpty()) {
            QMessageBox::warning(this, "Trip Error", QString("No path between '%1' and '%2'.").arg(bestOrder[i], bestOrder[i+1]));
            ui->tripSummaryText->setText("No path between selected stadiums.");
            ui->totalDistanceLabel->setText("Total Distance: 0 miles");
            return;
        }
        if (i == 0) {
            fullPath += segmentPath;
        } else {
            for (int j = 1; j < segmentPath.size(); ++j) {
                fullPath.append(segmentPath[j]);
            }
        }
        totalDistance += dist;
    }
    QString summary = "Dream Vacation (Optimized Order, Shortest Paths):\n";
    for (int i = 0; i < fullPath.size(); ++i) {
        summary += fullPath[i];
        if (i < fullPath.size() - 1) summary += " -> ";
    }
    summary += QString("\n\nTotal Distance: %1 miles").arg(totalDistance, 0, 'f', 2);
    ui->tripSummaryText->setText(summary);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(totalDistance, 0, 'f', 2));
    // Update Trip Stadiums list
    ui->tripStadiumsList->clear();
    for (const QString& stadium : fullPath) {
        ui->tripStadiumsList->addItem(findTeamNameByStadium(stadium));
    }
    if (ui->tripStadiumsList->count() > 0) {
        ui->tripStadiumsList->setCurrentRow(0);
        updateSouvenirTableForSelectedStadium();
    }
}
void TripPlanner::on_dfsOracleButton_clicked() {
    // TODO: Implement DFS from Oracle Park logic
}
void TripPlanner::on_bfsTargetButton_clicked() {
    // TODO: Implement BFS from Target Field logic
}

void TripPlanner::refreshStadiumLists() {
    ui->startingStadiumCombo->clear();
    ui->availableStadiumsList->clear();
    if (ui->dfsBfsStartCombo) ui->dfsBfsStartCombo->clear();

    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    QVector<QString> teamNames;
    for (const auto& entry : entries) {
        teamNames.append(entry.first);
    }
    std::sort(teamNames.begin(), teamNames.end());
    for (const QString& team : teamNames) {
        ui->availableStadiumsList->addItem(team);
        ui->startingStadiumCombo->addItem(team);
        if (ui->dfsBfsStartCombo) ui->dfsBfsStartCombo->addItem(team);
    }
}

void TripPlanner::on_startingStadiumCombo_currentIndexChanged(const QString &stadium) {
    // Overwrite the first entry, keep the rest, no duplicates
    QList<QString> tripStadiums;
    if (!stadium.isEmpty()) tripStadiums.append(stadium);
    for (int i = 1; i < ui->tripStadiumsList->count(); ++i) {
        QString s = ui->tripStadiumsList->item(i)->text();
        if (s != stadium) tripStadiums.append(s);
    }
    ui->tripStadiumsList->clear();
    for (const QString& s : tripStadiums) ui->tripStadiumsList->addItem(s);
    updateSouvenirTableForSelectedStadium();
}

void TripPlanner::updateOverallSouvenirSummary() {
    double totalCost = 0.0;
    ui->souvenirCartTable->setRowCount(0);
    int row = 0;
    for (auto it = souvenirCart.begin(); it != souvenirCart.end(); ++it) {
        const QString& stadium = it.key();
        const QVector<QPair<QString, int>>& items = it.value();
        StadiumInfo info;
        stadiumMap.get(stadium, info);
        for (const auto& pair : items) {
            double price = 0.0;
            for (const auto& souvenir : info.souvenirs) {
                if (souvenir.first == pair.first) price = souvenir.second;
            }
            double cost = price * pair.second;
            totalCost += cost;
            ui->souvenirCartTable->insertRow(row);
            ui->souvenirCartTable->setItem(row, 0, new QTableWidgetItem(stadium));
            ui->souvenirCartTable->setItem(row, 1, new QTableWidgetItem(pair.first));
            ui->souvenirCartTable->setItem(row, 2, new QTableWidgetItem(QString::number(pair.second)));
            ui->souvenirCartTable->setItem(row, 3, new QTableWidgetItem(QString::number(price, 'f', 2)));
            ui->souvenirCartTable->setItem(row, 4, new QTableWidgetItem(QString::number(cost, 'f', 2)));
            row++;
        }
    }
    ui->totalCostLabel->setText(QString("Total Cost: $%1").arg(totalCost, 0, 'f', 2));
    if (row == 0) {
        ui->souvenirSummaryLabel->setText("Souvenir Summary: No souvenirs selected.");
    } else {
        ui->souvenirSummaryLabel->setText("Souvenir Summary: See table below.");
    }
}

void TripPlanner::updateAlgorithmUIVisibility() {
    if (!ui->algorithmCombo) return;
    // Clear the Trip Stadiums list and all trip-related UI when a new algorithm is selected
    ui->tripStadiumsList->clear();
    ui->tripSummaryText->clear();
    ui->totalDistanceLabel->setText("Total Distance: 0 miles");
    ui->totalCostLabel->setText("Total Cost: $0.00");
    if (ui->souvenirTable) ui->souvenirTable->setRowCount(0);
    if (ui->souvenirSummaryLabel) ui->souvenirSummaryLabel->setText("Souvenir Summary: No souvenirs selected.");
    QString selected = ui->algorithmCombo->currentText().toLower();
    bool showDfsBfs = selected.contains("dfs") || selected.contains("bfs");
    if (ui->dfsBfsStartLabel) ui->dfsBfsStartLabel->setVisible(showDfsBfs);
    if (ui->dfsBfsStartCombo) ui->dfsBfsStartCombo->setVisible(showDfsBfs);
}

void TripPlanner::on_planTripButton_clicked() {
    if (!ui->algorithmCombo) return;
    QString selected = ui->algorithmCombo->currentText().toLower();
    if (selected.contains("dijkstra")) {
        on_dijkstraButton_clicked();
    } else if (selected.contains("mst")) {
        on_mstButton_clicked();
    } else if (selected.contains("dfs")) {
        on_dfsButton_clicked();
    } else if (selected.contains("bfs")) {
        on_bfsButton_clicked();
    } else if (selected.contains("greedy") || selected.contains("visit all")) {
        on_greedyButton_clicked();
    } else if (selected.contains("custom order")) {
        on_customOrderTripButton_clicked();
    } else if (selected.contains("dream vacation")) {
        on_dreamVacationButton_clicked();
    } else {
        // Default: do nothing or show a message
        QMessageBox::information(this, "Trip Planner", "Please select a valid trip planning algorithm.");
    }
}

// Add this slot for removing souvenirs from the cart
template<typename T> static void erase_if(QMap<QString, QVector<QPair<QString, int>>>& map, T pred) {
    for (auto it = map.begin(); it != map.end(); ) {
        QVector<QPair<QString, int>>& items = it.value();
        items.erase(std::remove_if(items.begin(), items.end(), [&](const QPair<QString, int>& pair) { return pred(it.key(), pair); }), items.end());
        if (items.isEmpty()) it = map.erase(it); else ++it;
    }
}
void TripPlanner::on_removeSouvenirButton_clicked() {
    int row = ui->souvenirCartTable->currentRow();
    if (row < 0) return;
    QString stadium = ui->souvenirCartTable->item(row, 0)->text();
    QString souvenir = ui->souvenirCartTable->item(row, 1)->text();
    // Remove this souvenir from the cart
    erase_if(souvenirCart, [&](const QString& s, const QPair<QString, int>& pair) {
        return s == stadium && pair.first == souvenir;
    });
    updateOverallSouvenirSummary();
}

QString TripPlanner::findTeamNameByStadium(const QString& normalizedStadium) const {
    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    for (const auto& entry : entries) {
        if (StadiumGraph::normalizeStadiumName(entry.second.stadiumName) == normalizedStadium) {
            return entry.first; // team name
        }
    }
    return normalizedStadium; // fallback
} 