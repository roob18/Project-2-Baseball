#ifndef ADMINLOGINDIALOG_H
#define ADMINLOGINDIALOG_H

#include <QDialog>
#include "database.h"

namespace Ui {
class AdminLoginDialog;
}

class AdminLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminLoginDialog(Database* database, QWidget *parent = nullptr);
    ~AdminLoginDialog();

private slots:
    void on_loginButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AdminLoginDialog *ui;
    Database* db;
};

#endif // ADMINLOGINDIALOG_H 