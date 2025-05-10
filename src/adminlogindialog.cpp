#include "adminlogindialog.h"
#include "ui_adminlogindialog.h"
#include "adminpanel.h"
#include <QMessageBox>

AdminLoginDialog::AdminLoginDialog(Database* database, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AdminLoginDialog)
    , db(database)
{
    ui->setupUi(this);
    setWindowTitle("Admin Login");
}

AdminLoginDialog::~AdminLoginDialog()
{
    delete ui;
}

void AdminLoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (db->validateAdmin(username, password)) {
        AdminPanel* adminPanel = new AdminPanel(db, this);
        adminPanel->setAttribute(Qt::WA_DeleteOnClose);
        adminPanel->show();
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}

void AdminLoginDialog::on_cancelButton_clicked()
{
    reject();
} 