#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "adminlogindialog.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , adminManager(new AdminManager(this))
    , isAdminLoggedIn(false)
{
    ui->setupUi(this);
    setupAdminUI();
    showAdminOnlyUI(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupAdminUI()
{
    // Add admin login button to the main window
    QPushButton *adminLoginButton = new QPushButton("Admin Login", this);
    ui->toolBar->addWidget(adminLoginButton);
    connect(adminLoginButton, &QPushButton::clicked, this, &MainWindow::on_adminLoginButton_clicked);

    // Add admin-only buttons (initially hidden)
    QPushButton *addStadiumButton = new QPushButton("Add Stadium", this);
    QPushButton *updateSouvenirPriceButton = new QPushButton("Update Souvenir Price", this);
    QPushButton *addTraditionalSouvenirButton = new QPushButton("Add Traditional Souvenir", this);
    QPushButton *deleteTraditionalSouvenirButton = new QPushButton("Delete Traditional Souvenir", this);

    ui->toolBar->addWidget(addStadiumButton);
    ui->toolBar->addWidget(updateSouvenirPriceButton);
    ui->toolBar->addWidget(addTraditionalSouvenirButton);
    ui->toolBar->addWidget(deleteTraditionalSouvenirButton);

    connect(addStadiumButton, &QPushButton::clicked, this, &MainWindow::on_addStadiumButton_clicked);
    connect(updateSouvenirPriceButton, &QPushButton::clicked, this, &MainWindow::on_updateSouvenirPriceButton_clicked);
    connect(addTraditionalSouvenirButton, &QPushButton::clicked, this, &MainWindow::on_addTraditionalSouvenirButton_clicked);
    connect(deleteTraditionalSouvenirButton, &QPushButton::clicked, this, &MainWindow::on_deleteTraditionalSouvenirButton_clicked);
}

void MainWindow::showAdminOnlyUI(bool show)
{
    // Show/hide admin-only buttons
    for (int i = 1; i < ui->toolBar->actions().size(); ++i) {
        ui->toolBar->actions()[i]->setVisible(show);
    }
}

void MainWindow::on_adminLoginButton_clicked()
{
    AdminLoginDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        isAdminLoggedIn = true;
        showAdminOnlyUI(true);
        QMessageBox::information(this, "Success", "Admin login successful");
    }
}

void MainWindow::on_addStadiumButton_clicked()
{
    if (!isAdminLoggedIn) {
        QMessageBox::warning(this, "Access Denied", "Admin login required");
        return;
    }

    QString stadiumName = QInputDialog::getText(this, "Add Stadium", "Enter stadium name:");
    if (stadiumName.isEmpty()) return;

    QString teamName = QInputDialog::getText(this, "Add Stadium", "Enter team name:");
    if (teamName.isEmpty()) return;

    QString souvenirFile = QFileDialog::getOpenFileName(this, "Select Souvenir File", "", "Text Files (*.txt);;All Files (*)");
    if (souvenirFile.isEmpty()) return;

    if (adminManager->addNewStadium(stadiumName, teamName, souvenirFile)) {
        QMessageBox::information(this, "Success", "Stadium added successfully");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add stadium");
    }
}

void MainWindow::on_updateSouvenirPriceButton_clicked()
{
    if (!isAdminLoggedIn) {
        QMessageBox::warning(this, "Access Denied", "Admin login required");
        return;
    }

    QString souvenirName = QInputDialog::getText(this, "Update Price", "Enter souvenir name:");
    if (souvenirName.isEmpty()) return;

    bool ok;
    double newPrice = QInputDialog::getDouble(this, "Update Price", "Enter new price:", 0, 0, 1000, 2, &ok);
    if (!ok) return;

    if (adminManager->updateTraditionalSouvenirPrice(souvenirName, newPrice)) {
        QMessageBox::information(this, "Success", "Price updated successfully");
    } else {
        QMessageBox::critical(this, "Error", "Failed to update price");
    }
}

void MainWindow::on_addTraditionalSouvenirButton_clicked()
{
    if (!isAdminLoggedIn) {
        QMessageBox::warning(this, "Access Denied", "Admin login required");
        return;
    }

    QString souvenirName = QInputDialog::getText(this, "Add Souvenir", "Enter souvenir name:");
    if (souvenirName.isEmpty()) return;

    bool ok;
    double price = QInputDialog::getDouble(this, "Add Souvenir", "Enter price:", 0, 0, 1000, 2, &ok);
    if (!ok) return;

    if (adminManager->addTraditionalSouvenir(souvenirName, price)) {
        QMessageBox::information(this, "Success", "Souvenir added successfully");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add souvenir");
    }
}

void MainWindow::on_deleteTraditionalSouvenirButton_clicked()
{
    if (!isAdminLoggedIn) {
        QMessageBox::warning(this, "Access Denied", "Admin login required");
        return;
    }

    QString souvenirName = QInputDialog::getText(this, "Delete Souvenir", "Enter souvenir name:");
    if (souvenirName.isEmpty()) return;

    if (adminManager->deleteTraditionalSouvenir(souvenirName)) {
        QMessageBox::information(this, "Success", "Souvenir deleted successfully");
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete souvenir");
    }
}
