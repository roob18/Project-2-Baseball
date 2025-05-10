#ifndef SOUVENIRDIALOG_H
#define SOUVENIRDIALOG_H

#include <QDialog>
#include "database.h"

namespace Ui {
class SouvenirDialog;
}

class SouvenirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SouvenirDialog(Database* database, const QString& teamName, QWidget *parent = nullptr);
    ~SouvenirDialog();

private slots:
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();
    void on_closeButton_clicked();

private:
    void loadSouvenirs();
    Ui::SouvenirDialog *ui;
    Database* db;
    QString teamName;
};

#endif // SOUVENIRDIALOG_H 