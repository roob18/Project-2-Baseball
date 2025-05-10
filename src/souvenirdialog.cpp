#include "souvenirdialog.h"
#include "ui_souvenirdialog.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QStyledItemDelegate>

SouvenirDialog::SouvenirDialog(Database* database, const QString& teamName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SouvenirDialog)
    , db(database)
    , teamName(teamName)
{
    ui->setupUi(this);
    setWindowTitle(teamName + " Souvenirs");
    
    // Set up the table
    ui->souvenirTable->setColumnCount(2);
    ui->souvenirTable->setHorizontalHeaderLabels(QStringList() << "Item" << "Price");
    ui->souvenirTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->souvenirTable->setSelectionMode(QAbstractItemView::SingleSelection);
    
    loadSouvenirs();
}

SouvenirDialog::~SouvenirDialog()
{
    delete ui;
}

void SouvenirDialog::loadSouvenirs()
{
    ui->souvenirTable->setRowCount(0);
    QVector<QPair<QString, double>> souvenirs = db->getSouvenirs(teamName);
    
    for (const auto& souvenir : souvenirs) {
        int row = ui->souvenirTable->rowCount();
        ui->souvenirTable->insertRow(row);
        
        QTableWidgetItem* nameItem = new QTableWidgetItem(souvenir.first);
        QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(souvenir.second, 'f', 2));
        
        ui->souvenirTable->setItem(row, 0, nameItem);
        ui->souvenirTable->setItem(row, 1, priceItem);
    }
    
    ui->souvenirTable->resizeColumnsToContents();
}

void SouvenirDialog::on_addButton_clicked()
{
    int row = ui->souvenirTable->rowCount();
    ui->souvenirTable->insertRow(row);
    
    QTableWidgetItem* nameItem = new QTableWidgetItem();
    QTableWidgetItem* priceItem = new QTableWidgetItem("0.00");
    
    ui->souvenirTable->setItem(row, 0, nameItem);
    ui->souvenirTable->setItem(row, 1, priceItem);
    
    ui->souvenirTable->editItem(nameItem);
}

void SouvenirDialog::on_editButton_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->souvenirTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, tr("No Selection"), 
                           tr("Please select a souvenir to edit."));
        return;
    }
    
    ui->souvenirTable->editItem(selectedItems.first());
}

void SouvenirDialog::on_deleteButton_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->souvenirTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, tr("No Selection"), 
                           tr("Please select a souvenir to delete."));
        return;
    }
    
    if (QMessageBox::question(this, tr("Confirm Delete"),
                            tr("Are you sure you want to delete this souvenir?"),
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        int row = selectedItems.first()->row();
        QString itemName = ui->souvenirTable->item(row, 0)->text();
        
        if (db->deleteSouvenir(teamName, itemName)) {
            ui->souvenirTable->removeRow(row);
        } else {
            QMessageBox::warning(this, tr("Error"),
                               tr("Could not delete souvenir."));
        }
    }
}

void SouvenirDialog::on_closeButton_clicked()
{
    // Save any pending changes
    for (int row = 0; row < ui->souvenirTable->rowCount(); ++row) {
        QString itemName = ui->souvenirTable->item(row, 0)->text();
        double price = ui->souvenirTable->item(row, 1)->text().toDouble();
        
        // Check if this is a new item or an existing one
        bool exists = false;
        QVector<QPair<QString, double>> existingSouvenirs = db->getSouvenirs(teamName);
        for (const auto& souvenir : existingSouvenirs) {
            if (souvenir.first == itemName) {
                exists = true;
                if (souvenir.second != price) {
                    db->updateSouvenirPrice(teamName, itemName, price);
                }
                break;
            }
        }
        
        if (!exists) {
            db->addSouvenir(teamName, itemName, price);
        }
    }
    
    close();
} 