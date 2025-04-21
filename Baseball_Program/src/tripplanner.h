#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include "trip.h"
#include "hashmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TripPlanner; }
QT_END_NAMESPACE

class TripPlanner : public QDialog
{
    Q_OBJECT

public:
    explicit TripPlanner(const HashMap<QString, StadiumInfo>& stadiumMap, QWidget *parent = nullptr);
    ~TripPlanner();

private slots:
    void on_addStopButton_clicked();
    void on_removeStopButton_clicked();
    void on_addSouvenirButton_clicked();
    void on_clearTripButton_clicked();
    void on_sortButton_clicked();
    void on_filterLeagueCombo_currentIndexChanged(const QString &league);
    void updateTripInfo();

private:
    Ui::TripPlanner *ui;
    Trip currentTrip;
    const HashMap<QString, StadiumInfo>& stadiumMap;
    
    void setupUi();
    void updateStopList();
    void updateTotalCost();
    void updateTotalDistance();
};

#endif // TRIPPLANNER_H 