#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

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
    
    // Populate the combo box with team names
    QSqlQuery query = db->getAllTeamsSortedByTeamName();
    while (query.next()) {
        ui->teamComboBox->addItem(query.value(0).toString());
    }
    
    // Setup all button connections
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete db;
}

void MainWindow::setupConnections()
{
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
    connect(ui->importCSVButton, &QPushButton::clicked, this, &MainWindow::onImportCSV);
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
    
    int row = 0;
    while (query.next()) {
        ui->resultsTable->insertRow(row);
        for (int col = 0; col < headers.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->resultsTable->setItem(row, col, item);
        }
        row++;
    }
    
    ui->resultsTable->resizeColumnsToContents();
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
    displayQueryResults(query, headers);
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

void MainWindow::onImportCSV()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Import CSV File"), "",
        tr("CSV Files (*.csv);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    if (db->importFromCSV(fileName)) {
        QMessageBox::information(this, tr("Success"),
            tr("Data imported successfully!"));
            
        // Refresh the combo box with updated team names
        ui->teamComboBox->clear();
        QSqlQuery query = db->getAllTeamsSortedByTeamName();
        while (query.next()) {
            ui->teamComboBox->addItem(query.value(0).toString());
        }
    } else {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to import data. Please check the CSV format."));
    }
} 