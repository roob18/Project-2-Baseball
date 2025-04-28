#ifndef SHORTESTTRIPDIALOG_H
#define SHORTESTTRIPDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QListWidget>
#include <QLabel>
#include "hashmap.h"
#include "trip.h"

namespace Ui {
class ShortestTripDialog;
}

class ShortestTripDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortestTripDialog(const HashMap<QString, StadiumInfo>& stadiumMap, QWidget *parent = nullptr);
    ~ShortestTripDialog();

private slots:
    void on_planTripButton_clicked();
    void on_selectAllButton_clicked();
    void on_deselectAllButton_clicked();

private:
    Ui::ShortestTripDialog *ui;
    const HashMap<QString, StadiumInfo>& stadiumMap;
    Trip trip;
    
    void setupUi();
    void updateRouteDisplay(const QVector<QString>& path);
    double calculateTotalDistance(const QVector<QString>& path);
};

#endif // SHORTESTTRIPDIALOG_H 