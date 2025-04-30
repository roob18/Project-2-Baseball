#ifndef SOUVENIRCHECKOUTDIALOG_H
#define SOUVENIRCHECKOUTDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include <QTableWidget>

class SouvenirCheckoutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SouvenirCheckoutDialog(const QString& teamName, QSqlDatabase db, QWidget *parent = nullptr);

private slots:
    void onCheckoutClicked();

private:
    QTableWidget* table;
    QString teamName;
    QSqlDatabase database;

    void loadSouvenirs();
};

#endif // SOUVENIRCHECKOUTDIALOG_H
