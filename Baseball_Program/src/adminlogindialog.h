#ifndef ADMINLOGINDIALOG_H
#define ADMINLOGINDIALOG_H

#include <QDialog>

namespace Ui {
class AdminLoginDialog;
}

class AdminLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminLoginDialog(QWidget *parent = nullptr);
    ~AdminLoginDialog();

private slots:
    void on_loginButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AdminLoginDialog *ui;
    bool authenticateAdmin(const QString &username, const QString &password);
};

#endif // ADMINLOGINDIALOG_H 