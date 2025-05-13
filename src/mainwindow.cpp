#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "adminlogindialog.h"
#include "adminpanel.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = new Database();
    
    // Initialize database
    if (!db->initialize()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database!");
        return;
    }
    
    // Make combo box read-only
    ui->teamComboBox->setEditable(false);
    
    // Setup all button connections first
    setupConnections();
    
    // Populate the combo box with team names
    QSqlQuery query = db->getAllTeamsSortedByTeamName();
    while (query.next()) {
        ui->teamComboBox->addItem(query.value(0).toString());
    }
    
    // Display initial team info
    if (ui->teamComboBox->count() > 0) {
        displayTeamInfo();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete db;
}

void MainWindow::setupConnections()
{
    // Connect combo box selection change to display team info
    connect(ui->teamComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::displayTeamInfo);
    
    // Connect all other buttons
    connect(ui->teamInfoButton, &QPushButton::clicked, this, &MainWindow::displayTeamInfo);
    connect(ui->sortByTeamButton, &QPushButton::clicked, this, &MainWindow::displayAllTeamsByTeamName);
    connect(ui->sortByStadiumButton, &QPushButton::clicked, this, &MainWindow::displayAllTeamsByStadiumName);
    connect(ui->americanLeagueButton, &QPushButton::clicked, this, &MainWindow::displayAmericanLeagueTeams);
    connect(ui->nationalLeagueButton, &QPushButton::clicked, this, &MainWindow::displayNationalLeagueTeams);
    connect(ui->typologyButton, &QPushButton::clicked, this, &MainWindow::displayTeamsByTypology);
    connect(ui->openRoofButton, &QPushButton::clicked, this, &MainWindow::displayOpenRoofTeams);
    connect(ui->dateOpenedButton, &QPushButton::clicked, this, &MainWindow::displayTeamsByDateOpened);
    connect(ui->capacityButton, &QPushButton::clicked, this, &MainWindow::displayTeamsByCapacity);
    connect(ui->maxCenterFieldButton, &QPushButton::clicked, this, &MainWindow::displayGreatestCenterField);
    connect(ui->minCenterFieldButton, &QPushButton::clicked, this, &MainWindow::displaySmallestCenterField);
    connect(ui->viewSouvenirsButton, &QPushButton::clicked, this, &MainWindow::viewTeamSouvenirs);
    connect(ui->resetDatabaseButton, &QPushButton::clicked, this, &MainWindow::on_resetDatabaseButton_clicked);
}

void MainWindow::clearResults()
{
    ui->resultsTable->clear();
    ui->resultsTable->setRowCount(0);
    ui->resultsTable->setColumnCount(0);
}

void MainWindow::displayQueryResults(QSqlQuery &query, const QStringList &headers)
{
    clearResults();
    
    ui->resultsTable->setColumnCount(headers.size());
    ui->resultsTable->setHorizontalHeaderLabels(headers);
    
    // Make the table read-only
    ui->resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    int row = 0;
    while (query.next()) {
        ui->resultsTable->insertRow(row);
        for (int col = 0; col < headers.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            // Make each item non-editable
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->resultsTable->setItem(row, col, item);
        }
        row++;
    }
    
    ui->resultsTable->resizeColumnsToContents();
    
    // Disable drag and drop
    ui->resultsTable->setDragDropMode(QAbstractItemView::NoDragDrop);
    
    // Disable row moving
    ui->resultsTable->verticalHeader()->setSectionsMovable(false);
    
    // Disable column moving
    ui->resultsTable->horizontalHeader()->setSectionsMovable(false);
}

void MainWindow::displayTeamInfo()
{
    QString teamName = ui->teamComboBox->currentText();
    QSqlQuery query = db->getTeamInfo(teamName);
    
    QStringList headers = {"Team Name", "Stadium", "Capacity", "Location", "Surface",
                         "League", "Opened", "Center Field", "Typology", "Roof"};
    displayQueryResults(query, headers);
}

void MainWindow::displayAllTeamsByTeamName()
{
    QSqlQuery query = db->getAllTeamsSortedByTeamName();
    QStringList headers = {"Team Name", "Stadium"};
    displayQueryResults(query, headers);
}

void MainWindow::displayAllTeamsByStadiumName()
{
    QSqlQuery query = db->getAllTeamsSortedByStadiumName();
    QStringList headers = {"Team Name", "Stadium"};
    displayQueryResults(query, headers);
}

void MainWindow::displayAmericanLeagueTeams()
{
    QSqlQuery query = db->getAmericanLeagueTeams();
    QStringList headers = {"Team Name", "Stadium"};
    displayQueryResults(query, headers);
}

void MainWindow::displayNationalLeagueTeams()
{
    QSqlQuery query = db->getNationalLeagueTeams();
    QStringList headers = {"Team Name", "Stadium"};
    displayQueryResults(query, headers);
}

void MainWindow::displayTeamsByTypology()
{
    QSqlQuery query = db->getTeamsByTypology();
    QStringList headers = {"Stadium", "Team Name", "Typology"};
    displayQueryResults(query, headers);
}

void MainWindow::displayOpenRoofTeams()
{
    QSqlQuery query = db->getOpenRoofTeams();
    QStringList headers = {"Team Name"};
    displayQueryResults(query, headers);
}

void MainWindow::displayTeamsByDateOpened()
{
    QSqlQuery query = db->getTeamsByDateOpened();
    QStringList headers = {"Stadium", "Team Name", "Date Opened"};
    displayQueryResults(query, headers);
}

void MainWindow::displayTeamsByCapacity()
{
    QSqlQuery query = db->getTeamsByCapacity();
    QStringList headers = {"Stadium", "Team Name", "Capacity"};
    clearResults();
    ui->resultsTable->setColumnCount(headers.size());
    ui->resultsTable->setHorizontalHeaderLabels(headers);
    ui->resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    int row = 0;
    int totalCapacity = 0;
    while (query.next()) {
        ui->resultsTable->insertRow(row);
        for (int col = 0; col < headers.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->resultsTable->setItem(row, col, item);
        }
        // Sum the capacity (column 2)
        bool ok = false;
        int cap = query.value(2).toInt(&ok);
        if (ok) totalCapacity += cap;
        row++;
    }
    // Add summary row
    ui->resultsTable->insertRow(row);
    QTableWidgetItem *totalLabel1 = new QTableWidgetItem("Total");
    totalLabel1->setFlags(totalLabel1->flags() & ~Qt::ItemIsEditable);
    QTableWidgetItem *totalLabel2 = new QTableWidgetItem("");
    totalLabel2->setFlags(totalLabel2->flags() & ~Qt::ItemIsEditable);
    QTableWidgetItem *totalValue = new QTableWidgetItem(QString::number(totalCapacity));
    totalValue->setFlags(totalValue->flags() & ~Qt::ItemIsEditable);
    ui->resultsTable->setItem(row, 0, totalLabel1);
    ui->resultsTable->setItem(row, 1, totalLabel2);
    ui->resultsTable->setItem(row, 2, totalValue);

    ui->resultsTable->resizeColumnsToContents();
    ui->resultsTable->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui->resultsTable->verticalHeader()->setSectionsMovable(false);
    ui->resultsTable->horizontalHeader()->setSectionsMovable(false);
}

void MainWindow::displayGreatestCenterField()
{
    QSqlQuery query = db->getTeamsWithGreatestCenterField();
    QStringList headers = {"Stadium", "Team Name", "Distance to Center"};
    displayQueryResults(query, headers);
}

void MainWindow::displaySmallestCenterField()
{
    QSqlQuery query = db->getTeamsWithSmallestCenterField();
    QStringList headers = {"Stadium", "Team Name", "Distance to Center"};
    displayQueryResults(query, headers);
}

void MainWindow::viewTeamSouvenirs()
{
    QString selectedTeam = ui->teamComboBox->currentText();
    if (selectedTeam.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a team first!");
        return;
    }

    QSqlQuery query(db->database());
    query.prepare("SELECT item_name, price FROM souvenirs WHERE team_name = :team");
    query.bindValue(":team", selectedTeam);

    if (query.exec()) {
        QStringList headers;
        headers << "Souvenir" << "Price ($)";
        displayQueryResults(query, headers);
    } else {
        QMessageBox::critical(this, "Error", "Failed to fetch souvenirs: " + query.lastError().text());
    }
}

void MainWindow::refreshData()
{
    // Clear and repopulate the combo box
        ui->teamComboBox->clear();
        QSqlQuery query = db->getAllTeamsSortedByTeamName();
        while (query.next()) {
            ui->teamComboBox->addItem(query.value(0).toString());
        }

    // Refresh the current display
    if (ui->teamComboBox->count() > 0) {
        displayTeamInfo();
    }
}

void MainWindow::on_adminLoginButton_clicked()
{
    AdminLoginDialog loginDialog(db, stadiumGraph, this);
    if (loginDialog.exec() == QDialog::Accepted) {
        db->loadStadiumMap(); // Reload stadium and souvenir data after admin changes
        refreshData();        // Update UI
    }
}

void MainWindow::on_tripPlannerButton_clicked()
{
    if (!stadiumGraph) {
        QMessageBox::warning(this, "Error", "Stadium graph not loaded.");
        return;
    }
    TripPlanner* planner = new TripPlanner(db->getStadiumMap(), stadiumGraph, this);
    planner->exec();
    delete planner;
}

void MainWindow::on_resetDatabaseButton_clicked()
{
    // Confirm with the user
    if (QMessageBox::question(this, "Reset Database", "Are you sure you want to reset the database? All imported data will be lost.", QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
        return;

    // Close and delete the database file
    db->~Database();
    QFile dbFile("baseball.db");
    if (dbFile.exists()) {
        dbFile.remove();
    }

    // Restart the application
    qApp->exit(1000); // Use a custom exit code to indicate restart
}
