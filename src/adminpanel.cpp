#include "adminpanel.h"
#include "ui_adminpanel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>
#include <QPushButton>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QThread>

AdminPanel::AdminPanel(Database* database, StadiumGraph* stadiumGraph, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AdminPanel)
    , db(database)
    , stadiumGraph(stadiumGraph)
{
    ui->setupUi(this);
    setupUi();
    loadTeams();
    loadStadiumData(); // Load existing stadium data
    
    // Load initial souvenirs for the first team
    if (ui->teamComboBox->count() > 0) {
        QString initialTeam = ui->teamComboBox->currentText();
        qDebug() << "Loading initial souvenirs for team:" << initialTeam;
        loadSouvenirs(initialTeam);
    }
}

AdminPanel::~AdminPanel()
{
    delete ui;
}

void AdminPanel::setupUi()
{
    // Setup souvenir table
    ui->souvenirTable->setColumnCount(2);
    ui->souvenirTable->setHorizontalHeaderLabels({"Item Name", "Price ($)"});
    ui->souvenirTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->souvenirTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->souvenirTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Setup stadium table
    ui->stadiumTable->setColumnCount(10);
    ui->stadiumTable->setHorizontalHeaderLabels({
        "Team Name", "Stadium Name", "Capacity", "Location", 
        "Surface", "League", "Date Opened", "Center Field",
        "Typology", "Roof Type"
    });
    ui->stadiumTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->stadiumTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->stadiumTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    
    // Ensure the tables are visible and have a reasonable size
    ui->souvenirTable->setMinimumHeight(200);
    ui->stadiumTable->setMinimumHeight(300);
    ui->souvenirTable->horizontalHeader()->setStretchLastSection(true);
    ui->stadiumTable->horizontalHeader()->setStretchLastSection(true);
    ui->souvenirTable->verticalHeader()->setVisible(false);
    ui->stadiumTable->verticalHeader()->setVisible(false);

    // Add save changes button for stadiums
    QPushButton* saveButton = new QPushButton("Save Changes", this);
    ui->verticalLayout_2->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &AdminPanel::saveStadiumChanges);
}

void AdminPanel::loadTeams()
{
    ui->teamComboBox->clear();
    QSqlQuery query = db->getAllTeamsSortedByTeamName();
    while (query.next()) {
        QString teamName = query.value(0).toString();
        ui->teamComboBox->addItem(teamName);
        qDebug() << "Added team to combo box:" << teamName;
    }
}

void AdminPanel::loadSouvenirs(const QString &teamName)
{
    qDebug() << "Loading souvenirs for team:" << teamName;
    ui->souvenirTable->setRowCount(0);
    
    auto souvenirs = db->getSouvenirs(teamName);
    qDebug() << "Got souvenirs, adding to table...";
    
    int rowCount = 0;
    for (const auto &souvenir : souvenirs) {
        int row = ui->souvenirTable->rowCount();
        ui->souvenirTable->insertRow(row);
        QString itemName = souvenir.first;
        QString price = QString::number(souvenir.second, 'f', 2);
        ui->souvenirTable->setItem(row, 0, new QTableWidgetItem(itemName));
        ui->souvenirTable->setItem(row, 1, new QTableWidgetItem(price));
        rowCount++;
        qDebug() << "Added souvenir:" << itemName << "Price:" << price;
    }
    
    qDebug() << "Total souvenirs loaded:" << rowCount;
    ui->souvenirTable->resizeColumnsToContents();
}

void AdminPanel::on_importStadiumButton_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Import Stadium Data"), "",
        tr("CSV Files (*.csv);;All Files (*)"));

    if (fileNames.isEmpty())
        return;

    // Clear the stadium table
    ui->stadiumTable->setRowCount(0);

    // Start the import process
    if (db->importFromCSV(fileNames)) {
        // Load all teams into the table
        QSqlQuery query = db->getAllTeamsSortedByTeamName();
        while (query.next()) {
            int row = ui->stadiumTable->rowCount();
            ui->stadiumTable->insertRow(row);

            // Get full team info
            QSqlQuery teamInfo = db->getTeamInfo(query.value(0).toString());
            if (teamInfo.next()) {
                for (int col = 0; col < 10; ++col) {
                    QTableWidgetItem* item = new QTableWidgetItem(teamInfo.value(col).toString());
                    ui->stadiumTable->setItem(row, col, item);
                }
            }
        }

        QMessageBox::information(this, tr("Success"),
            tr("Stadium data imported successfully! You can now review and edit the data."));
        loadTeams();
    } else {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to import stadium data. Please check the CSV format."));
    }
}

void AdminPanel::saveStadiumChanges()
{
    db->database().transaction();
    bool success = true;

    for (int row = 0; row < ui->stadiumTable->rowCount(); ++row) {
        QString teamName = ui->stadiumTable->item(row, 0)->text();
        QString stadiumName = ui->stadiumTable->item(row, 1)->text();
        
        // Get capacity text and remove any commas
        QString capacityText = ui->stadiumTable->item(row, 2)->text().remove(',');
        bool ok;
        int capacity = capacityText.toInt(&ok);
        
        if (!ok || capacity < 0) {
            QMessageBox::critical(this, "Error", 
                QString("Invalid capacity value for %1: %2\nPlease enter a valid number.")
                .arg(teamName)
                .arg(ui->stadiumTable->item(row, 2)->text()));
            db->database().rollback();
            return;
        }

        QString location = ui->stadiumTable->item(row, 3)->text();
        QString surface = ui->stadiumTable->item(row, 4)->text();
        QString league = ui->stadiumTable->item(row, 5)->text();
        QString dateOpened = ui->stadiumTable->item(row, 6)->text();
        QString centerFieldStr = ui->stadiumTable->item(row, 7)->text();
        QString typology = ui->stadiumTable->item(row, 8)->text();
        QString roof = ui->stadiumTable->item(row, 9)->text();

        // Extract center field distance (assuming format "XXX feet (YY m)")
        QRegularExpression feetRegex("(\\d+)\\s*(?:feet|$)");
        auto feetMatch = feetRegex.match(centerFieldStr);
        int centerField = 0;
        if (feetMatch.hasMatch()) {
            centerField = feetMatch.captured(1).toInt();
        }

        if (!db->insertTeam(teamName, stadiumName, capacity, location, surface,
                           league, dateOpened, centerField, typology, roof)) {
            success = false;
            break;
        }
    }

    if (success && db->database().commit()) {
        QMessageBox::information(this, "Success", "Changes saved successfully!");
        db->reloadStadiumData(); // Use the new public method
        emit dataChanged(); // Emit signal when changes are saved successfully
    } else {
        db->database().rollback();
        QMessageBox::critical(this, "Error", "Failed to save changes. Please try again.");
    }
}

void AdminPanel::on_addSouvenirButton_clicked()
{
    QString teamName = ui->teamComboBox->currentText();
    if (teamName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a team first!");
        return;
    }

    bool ok;
    QString itemName = QInputDialog::getText(this, "Add Souvenir",
                                         "Enter souvenir name:", QLineEdit::Normal,
                                         "", &ok);
    if (!ok || itemName.isEmpty())
        return;

    double price = QInputDialog::getDouble(this, "Add Souvenir",
                                       "Enter price:", 0.00, 0.00, 10000.00, 2, &ok);
    if (!ok)
        return;

    QSqlQuery query(db->database());
    query.prepare("INSERT INTO souvenirs (team_name, item_name, price) VALUES (:team, :item, :price)");
    query.bindValue(":team", teamName);
    query.bindValue(":item", itemName);
    query.bindValue(":price", price);

    if (query.exec()) {
        loadSouvenirs(teamName);
    } else {
        QMessageBox::critical(this, "Error", "Failed to add souvenir: " + query.lastError().text());
    }
}

void AdminPanel::on_editSouvenirButton_clicked()
{
    QModelIndexList selection = ui->souvenirTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a souvenir to edit!");
        return;
    }

    QString teamName = ui->teamComboBox->currentText();
    QString currentItemName = ui->souvenirTable->item(selection.first().row(), 0)->text();
    double currentPrice = ui->souvenirTable->item(selection.first().row(), 1)->text().toDouble();

    bool ok;
    double newPrice = QInputDialog::getDouble(this, "Edit Souvenir",
                                          "Enter new price for " + currentItemName + ":",
                                          currentPrice, 0.00, 10000.00, 2, &ok);
    if (!ok)
        return;

    QSqlQuery query(db->database());
    query.prepare("UPDATE souvenirs SET price = :price WHERE team_name = :team AND item_name = :item");
    query.bindValue(":price", newPrice);
    query.bindValue(":team", teamName);
    query.bindValue(":item", currentItemName);

    if (query.exec()) {
        loadSouvenirs(teamName);
    } else {
        QMessageBox::critical(this, "Error", "Failed to update souvenir: " + query.lastError().text());
    }
}

void AdminPanel::on_deleteSouvenirButton_clicked()
{
    QModelIndexList selection = ui->souvenirTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a souvenir to delete!");
        return;
    }

    QString teamName = ui->teamComboBox->currentText();
    QString itemName = ui->souvenirTable->item(selection.first().row(), 0)->text();

    if (QMessageBox::question(this, "Confirm Delete",
                            "Are you sure you want to delete " + itemName + "?",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        return;
    }

    QSqlQuery query(db->database());
    query.prepare("DELETE FROM souvenirs WHERE team_name = :team AND item_name = :item");
    query.bindValue(":team", teamName);
    query.bindValue(":item", itemName);

    if (query.exec()) {
        loadSouvenirs(teamName);
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete souvenir: " + query.lastError().text());
    }
}

void AdminPanel::on_teamComboBox_currentIndexChanged(const QString &teamName)
{
    loadSouvenirs(teamName);
}

void AdminPanel::on_closeButton_clicked()
{
    accept();
}

void AdminPanel::loadStadiumData()
{
    // Clear the stadium table
    ui->stadiumTable->setRowCount(0);

    // Load all teams into the table
    QSqlQuery query = db->getAllTeamsSortedByTeamName();
    while (query.next()) {
        int row = ui->stadiumTable->rowCount();
        ui->stadiumTable->insertRow(row);

        // Get full team info
        QSqlQuery teamInfo = db->getTeamInfo(query.value(0).toString());
        if (teamInfo.next()) {
            for (int col = 0; col < 10; ++col) {
                QTableWidgetItem* item = new QTableWidgetItem(teamInfo.value(col).toString());
                ui->stadiumTable->setItem(row, col, item);
            }
        }
    }
    ui->stadiumTable->resizeColumnsToContents();
}

void AdminPanel::on_importDistancesButton_clicked() {
    qDebug() << "Starting importDistancesButton_clicked";
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Import Distances CSV", "", "CSV Files (*.csv)");
    qDebug() << "Files selected:" << filenames;
    if (filenames.isEmpty()) {
        qDebug() << "No file selected, returning.";
        return;
    }

    if (!stadiumGraph) {
        qDebug() << "stadiumGraph is null!";
        QMessageBox::critical(this, "Error", "Stadium graph is not available.");
        return;
    }

    if (stadiumGraph->loadMultipleCSVs(filenames)) {
        QMessageBox::information(this, "Import Successful", "Distances imported successfully.");
        // Debug: Print unreachable stadiums and missing paths
        stadiumGraph->debugPrintUnreachableStadiums();
        stadiumGraph->debugPrintAllMissingPaths();
    } else {
        QMessageBox::critical(this, "Import Error", "Failed to import distances from CSV(s).");
    }
}

void AdminPanel::on_addStadiumButton_clicked()
{
    // (Stub) Launch a dialog (AddStadiumDialog) to add a new stadium.
    // (Assume that AddStadiumDialog is a new dialog class that you will create later.)
    // For example:
    // AddStadiumDialog addDialog(db, stadiumGraph, this);
    // if (addDialog.exec() == QDialog::Accepted) {
    //     // (Stub) Insert the new stadium (e.g. addDialog.getStadiumName()) into the database (and stadiumGraph) and update stadiumCombo (and stadiumTable).
    //     db->insertStadium(addDialog.getStadiumName(), /* other stadium details */);
    //     stadiumGraph->addStadium(addDialog.getStadiumName());
    //     loadStadiums(); // (Stub) Reload stadiumCombo and stadiumTable.
    // }
    QMessageBox::information(this, "Add Stadium", "Add Stadium Dialog (stub) – Insert a new stadium (and edit its contents) here.");
} 