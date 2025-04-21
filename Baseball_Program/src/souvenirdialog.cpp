#include "souvenirdialog.h"
#include "ui_souvenirdialog.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QStyledItemDelegate>

SouvenirDialog::SouvenirDialog(const QString &teamName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SouvenirDialog),
    model(new QSqlTableModel(this)),
    currentTeam(teamName)
{
    ui->setupUi(this);
    setWindowTitle(tr("Manage Souvenirs - %1").arg(teamName));
    setupModel();
}

SouvenirDialog::~SouvenirDialog()
{
    delete ui;
}

void SouvenirDialog::setupModel()
{
    model->setTable("souvenirs");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Join with Teams table and filter by team name
    model->setFilter(QString("team_id IN (SELECT id FROM Teams WHERE team_name = '%1')")
                    .arg(currentTeam.replace("'", "''")));  // Escape single quotes
    
    if (!model->select()) {
        QMessageBox::critical(this, tr("Database Error"),
                            tr("Could not load souvenirs: %1")
                            .arg(model->lastError().text()));
        return;
    }
    
    // Set headers
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Team ID"));
    model->setHeaderData(2, Qt::Horizontal, tr("Item"));
    model->setHeaderData(3, Qt::Horizontal, tr("Price"));
    
    ui->souvenirTableView->setModel(model);
    ui->souvenirTableView->setColumnHidden(0, true); // Hide ID column
    ui->souvenirTableView->setColumnHidden(1, true); // Hide Team ID column
    ui->souvenirTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->souvenirTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Set the price delegate to show currency format
    ui->souvenirTableView->setItemDelegateForColumn(3, new QStyledItemDelegate(this));
    
    // Set column widths
    ui->souvenirTableView->setColumnWidth(2, 200); // Item name column
    ui->souvenirTableView->setColumnWidth(3, 100); // Price column
    
    // Enable sorting
    ui->souvenirTableView->setSortingEnabled(true);
    ui->souvenirTableView->sortByColumn(2, Qt::AscendingOrder); // Sort by item name
}

void SouvenirDialog::on_addButton_clicked()
{
    int row = model->rowCount();
    if (!model->insertRow(row)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Could not add new souvenir: %1")
                           .arg(model->lastError().text()));
        return;
    }
    
    // Set the team_id for the new row
    QSqlQuery query;
    query.prepare("SELECT id FROM Teams WHERE team_name = ?");
    query.addBindValue(currentTeam);
    if (query.exec() && query.next()) {
        int teamId = query.value(0).toInt();
        model->setData(model->index(row, 1), teamId);  // Set team_id
    }
}

void SouvenirDialog::on_editButton_clicked()
{
    QModelIndex currentIndex = ui->souvenirTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("No Selection"), 
                           tr("Please select a souvenir to edit."));
        return;
    }
    
    ui->souvenirTableView->edit(currentIndex);
}

void SouvenirDialog::on_deleteButton_clicked()
{
    QModelIndex currentIndex = ui->souvenirTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("No Selection"), 
                           tr("Please select a souvenir to delete."));
        return;
    }
    
    if (QMessageBox::question(this, tr("Confirm Delete"),
                            tr("Are you sure you want to delete this souvenir?"),
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (!model->removeRow(currentIndex.row())) {
            QMessageBox::warning(this, tr("Error"),
                               tr("Could not delete souvenir: %1")
                               .arg(model->lastError().text()));
            return;
        }
        model->submitAll();
    }
}

void SouvenirDialog::on_closeButton_clicked()
{
    if (model->isDirty()) {
        if (QMessageBox::question(this, tr("Save Changes"),
                                tr("Do you want to save your changes?"),
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if (!model->submitAll()) {
                QMessageBox::warning(this, tr("Database Error"),
                                   tr("Failed to save changes: %1")
                                   .arg(model->lastError().text()));
                return;
            }
        } else {
            model->revertAll();
        }
    }
    close();
} 