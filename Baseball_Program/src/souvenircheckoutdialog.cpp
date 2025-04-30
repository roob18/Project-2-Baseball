#include "souvenircheckoutdialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>

SouvenirCheckoutDialog::SouvenirCheckoutDialog(const QString& team, QSqlDatabase db, QWidget *parent)
    : QDialog(parent), teamName(team), database(db)
{
    setWindowTitle("Checkout Souvenirs - " + teamName);
    setMinimumWidth(450);
    setMinimumHeight(400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Souvenir", "Price ($)", "Quantity"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(table);

    QPushButton* checkoutButton = new QPushButton("Checkout", this);
    layout->addWidget(checkoutButton);
    connect(checkoutButton, &QPushButton::clicked, this, &SouvenirCheckoutDialog::onCheckoutClicked);

    loadSouvenirs();
}

void SouvenirCheckoutDialog::loadSouvenirs()
{
    QSqlQuery query(database);
    query.prepare("SELECT item_name, price FROM souvenirs WHERE team_name = :team");
    query.bindValue(":team", teamName);

    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            table->verticalHeader()->setDefaultSectionSize(50);

            QSpinBox* spinBox = new QSpinBox(this);
            spinBox->setRange(0, 100);
            spinBox->setAlignment(Qt::AlignCenter);
            table->setCellWidget(row, 2, spinBox);
            row++;
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to load souvenirs: " + query.lastError().text());
    }
}

void SouvenirCheckoutDialog::onCheckoutClicked()
{
    double total = 0.0;
    QStringList summary;

    for (int row = 0; row < table->rowCount(); ++row) {
        QString item = table->item(row, 0)->text();
        double price = table->item(row, 1)->text().toDouble();
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(table->cellWidget(row, 2));
        int quantity = spinBox->value();

        if (quantity > 0) {
            total += price * quantity;
            summary << QString("%1 x%2 ($%3)").arg(item).arg(quantity).arg(price * quantity, 0, 'f', 2);
        }
    }

    if (total > 0) {
        QMessageBox::information(this, "Checkout Complete",
                                 "You purchased:\n" + summary.join("\n") +
                                     QString("\n\nTotal: $%1").arg(total, 0, 'f', 2));
        accept(); // Close dialog
    } else {
        QMessageBox::warning(this, "No Items Selected", "Please select at least one souvenir.");
    }
}
