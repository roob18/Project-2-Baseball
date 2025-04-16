#include "tripplanner.h"
#include "ui_tripplanner.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtCore/QStringList>
#include <QtCore/QMap>

TripPlanner::TripPlanner(const HashMap<QString, StadiumInfo>& map, QWidget *parent)
    : QDialog(parent), ui(new Ui::TripPlanner), stadiumMap(map)
{
    setupUi();
    updateTripInfo();
}

TripPlanner::~TripPlanner()
{
    delete ui;
}

void TripPlanner::setupUi()
{
    ui->setupUi(this);
    
    // Populate league combo box
    ui->filterLeagueCombo->addItem("All");
    ui->filterLeagueCombo->addItem("American");
    ui->filterLeagueCombo->addItem("National");
    
    // Connect signals/slots
    connect(ui->addStopButton, &QPushButton::clicked, this, &TripPlanner::on_addStopButton_clicked);
    connect(ui->removeStopButton, &QPushButton::clicked, this, &TripPlanner::on_removeStopButton_clicked);
    connect(ui->addSouvenirButton, &QPushButton::clicked, this, &TripPlanner::on_addSouvenirButton_clicked);
    connect(ui->clearTripButton, &QPushButton::clicked, this, &TripPlanner::on_clearTripButton_clicked);
    connect(ui->sortButton, &QPushButton::clicked, this, &TripPlanner::on_sortButton_clicked);
    connect(ui->filterLeagueCombo, &QComboBox::currentTextChanged, this, &TripPlanner::on_filterLeagueCombo_currentIndexChanged);
}

void TripPlanner::on_addStopButton_clicked()
{
    bool ok;
    QStringList teams;
    
    // Get all entries from the HashMap
    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    for (const auto& entry : entries) {
        teams << entry.first;
    }
    
    teams.sort();  // Sort teams alphabetically
    
    QString team = QInputDialog::getItem(this, "Add Stop",
                                       "Select team to visit:", teams, 0, false, &ok);
    if (ok && !team.isEmpty()) {
        currentTrip.addStop(team);
        updateTripInfo();
    }
}

void TripPlanner::on_removeStopButton_clicked()
{
    QListWidgetItem* item = ui->stopList->currentItem();
    if (item) {
        int row = ui->stopList->row(item);
        delete item;
        
        // Remove the stop from the trip data
        QVector<TripStop>& stops = const_cast<QVector<TripStop>&>(currentTrip.getStops());
        if (row >= 0 && row < stops.size()) {
            stops.remove(row);
        }
        
        updateTripInfo();
    }
}

void TripPlanner::on_addSouvenirButton_clicked()
{
    QListWidgetItem* item = ui->stopList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Error", "Please select a stop first");
        return;
    }
    
    QString team = item->text();
    bool ok;
    
    // Create a map of souvenir names to their prices
    QMap<QString, double> souvenirPrices;
    souvenirPrices["Baseball Cap"] = 25.99;
    souvenirPrices["Baseball Bat"] = 35.99;
    souvenirPrices["Team Pennant"] = 12.99;
    souvenirPrices["Autographed Baseball"] = 49.99;
    
    QStringList souvenirs;
    for (auto it = souvenirPrices.begin(); it != souvenirPrices.end(); ++it) {
        souvenirs << QString("%1 - $%2").arg(it.key()).arg(it.value(), 0, 'f', 2);
    }
    
    QString souvenir = QInputDialog::getItem(this, "Add Souvenir",
                                           "Select souvenir:", souvenirs, 
                                           0, false, &ok);
    
    if (ok && !souvenir.isEmpty()) {
        int quantity = QInputDialog::getInt(this, "Quantity",
                                          "Enter quantity:", 1, 1, 100, 1, &ok);
        if (ok) {
            QString souvenirName = souvenir.split(" - ").first();
            double price = souvenirPrices[souvenirName];
            
            // Get the current stop and update its total cost
            int row = ui->stopList->currentRow();
            QVector<TripStop>& stops = const_cast<QVector<TripStop>&>(currentTrip.getStops());
            if (row >= 0 && row < stops.size()) {
                stops[row].purchasedSouvenirs.append(qMakePair(souvenirName, quantity));
                stops[row].totalCost += price * quantity;
            }
            
            updateTripInfo();
        }
    }
}

void TripPlanner::on_clearTripButton_clicked()
{
    ui->stopList->clear();
    currentTrip = Trip();
    updateTripInfo();
}

void TripPlanner::on_sortButton_clicked()
{
    currentTrip.sortByTeamName();
    updateStopList();
}

void TripPlanner::on_filterLeagueCombo_currentIndexChanged(const QString &league)
{
    ui->stopList->clear();
    
    if (league == "All") {
        const auto& stops = currentTrip.getStops();
        for (const auto& stop : stops) {
            ui->stopList->addItem(stop.teamName);
        }
        return;
    }
    
    QVector<TripStop> filteredStops = currentTrip.getTeamsByLeague(league, stadiumMap);
    for (const auto& stop : filteredStops) {
        ui->stopList->addItem(stop.teamName);
    }
}

void TripPlanner::updateStopList()
{
    ui->stopList->clear();
    const auto& stops = currentTrip.getStops();
    for (const auto& stop : stops) {
        QString displayText = stop.teamName;
        if (!stop.purchasedSouvenirs.isEmpty()) {
            displayText += QString(" (Souvenirs: %1)").arg(stop.purchasedSouvenirs.size());
        }
        ui->stopList->addItem(displayText);
    }
}

void TripPlanner::updateTotalCost()
{
    double cost = currentTrip.calculateTotalCost();
    ui->totalCostLabel->setText(QString("Total Cost: $%1").arg(cost, 0, 'f', 2));
}

void TripPlanner::updateTotalDistance()
{
    double distance = currentTrip.calculateTotalDistance(stadiumMap);
    ui->totalDistanceLabel->setText(QString("Total Distance: %1 miles").arg(distance, 0, 'f', 1));
}

void TripPlanner::updateTripInfo()
{
    updateStopList();
    updateTotalCost();
    updateTotalDistance();
} 