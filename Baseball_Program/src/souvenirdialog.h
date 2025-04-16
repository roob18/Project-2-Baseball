#ifndef SOUVENIRDIALOG_H
#define SOUVENIRDIALOG_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class SouvenirDialog;
}

class SouvenirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SouvenirDialog(const QString &teamName, QWidget *parent = nullptr);
    ~SouvenirDialog();

private slots:
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::SouvenirDialog *ui;
    QSqlTableModel *model;
    QString currentTeam;
    void setupModel();
};

#endif // SOUVENIRDIALOG_H 