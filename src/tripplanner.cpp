#include "tripplanner.h"
#include "ui_tripplanner.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

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
    updateAlgorithmUIVisibility();
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
}

void TripPlanner::on_mstButton_clicked()
{
    QVector<QPair<QString, QString>> mst;
    double totalWeight = stadiumGraph->minimumSpanningTree(mst);
    QString summary = "Minimum Spanning Tree:\n";
    int validEdgeCount = 0;
    for (const auto& edge : mst) {
        if (edge.first.isNull() || edge.second.isNull() || edge.first.isEmpty() || edge.second.isEmpty()) {
            continue; // skip invalid edges
        }
        summary += edge.first + " -- " + edge.second + "\n";
        ++validEdgeCount;
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
    QVector<QString> path;
    double distance = stadiumGraph->dfs(normalizedStart, path);
    if (distance < 0 || path.isEmpty()) {
        QMessageBox::warning(this, "Trip Error", "No path found from " + startStadium + ".");
        ui->tripSummaryText->setText("No path found from " + startStadium + ".");
        ui->totalDistanceLabel->setText("Total Distance: 0 miles");
        return;
    }
    if (path.size() < stadiumGraph->getStadiums().size()) {
        QMessageBox::warning(this, "Trip Warning", "Not all stadiums are reachable from " + startStadium + ".");
    }
    QString summary = "DFS Traversal (" + startStadium + "):\n";
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
        merged[pair.first] += pair.second;
    }
    // Add new purchases
    for (int i = 0; i < ui->souvenirTable->rowCount(); ++i) {
        QString name = ui->souvenirTable->item(i, 0)->text();
        int qty = ui->souvenirTable->item(i, 2)->text().toInt();
        if (qty > 0) merged[name] += qty;
    }
    QVector<QPair<QString, int>> mergedList;
    for (auto it = merged.begin(); it != merged.end(); ++it) {
        if (it.value() > 0) mergedList.append(qMakePair(it.key(), it.value()));
    }
    souvenirCart[stadium] = mergedList;
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
    // TODO: Implement greedy trip logic
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
    // TODO: Implement custom order trip logic
}
void TripPlanner::on_visitAllMarlinsButton_clicked() {
    // TODO: Implement visit all teams from Marlins Park logic
}
void TripPlanner::on_dreamVacationButton_clicked() {
    // TODO: Implement dream vacation (optimized order) logic
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
    QString summary;
    for (auto it = souvenirCart.begin(); it != souvenirCart.end(); ++it) {
        const QString& stadium = it.key();
        const QVector<QPair<QString, int>>& items = it.value();
        if (!items.isEmpty()) {
            summary += stadium + ": ";
            QStringList itemList;
            StadiumInfo info;
            stadiumMap.get(stadium, info);
            for (const auto& pair : items) {
                double price = 0.0;
                for (const auto& souvenir : info.souvenirs) {
                    if (souvenir.first == pair.first) price = souvenir.second;
                }
                double cost = price * pair.second;
                totalCost += cost;
                itemList << QString("%1 x%2 ($%3)").arg(pair.first).arg(pair.second).arg(cost, 0, 'f', 2);
            }
            summary += itemList.join(", ") + "\n";
        }
    }
    if (summary.isEmpty()) summary = "No souvenirs selected.";
    ui->souvenirSummaryLabel->setText("Souvenir Summary: " + summary.trimmed());
    ui->totalCostLabel->setText(QString("Total Cost: $%1").arg(totalCost, 0, 'f', 2));
}

void TripPlanner::updateAlgorithmUIVisibility() {
    if (!ui->algorithmCombo) return;
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