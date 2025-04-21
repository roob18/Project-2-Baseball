#include "adminlogindialog.h"
#include "ui_adminlogindialog.h"
#include <QMessageBox>

AdminLoginDialog::AdminLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminLoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("Admin Login");
}

AdminLoginDialog::~AdminLoginDialog()
{
    delete ui;
}

bool AdminLoginDialog::authenticateAdmin(const QString &username, const QString &password)
{
    // In a real application, this would check against a secure database
    // For now, we'll use a hardcoded admin account
    return (username == "admin" && password == "admin123");
}

void AdminLoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (authenticateAdmin(username, password)) {
        accept(); // Close dialog and return QDialog::Accepted
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password");
    }
}

void AdminLoginDialog::on_cancelButton_clicked()
{
    reject(); // Close dialog and return QDialog::Rejected
} 