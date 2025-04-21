#include "adminpanel.h"
#include "ui_adminpanel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>

AdminPanel::AdminPanel(Database* database, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AdminPanel)
    , db(database)
{
    ui->setupUi(this);
    setupUi();
    loadTeams();
    
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
    
    // Ensure the table is visible and has a reasonable size
    ui->souvenirTable->setMinimumHeight(200);
    ui->souvenirTable->horizontalHeader()->setStretchLastSection(true);
    ui->souvenirTable->verticalHeader()->setVisible(false);
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
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Import Stadium Data"), "",
        tr("CSV Files (*.csv);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    if (db->importFromCSV(fileName)) {
        QMessageBox::information(this, tr("Success"),
            tr("Stadium data imported successfully!"));
        loadTeams();
    } else {
        QMessageBox::critical(this, tr("Error"),
            tr("Failed to import stadium data. Please check the CSV format."));
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