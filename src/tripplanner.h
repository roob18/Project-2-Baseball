#ifndef TRIPPLANNER_H
#define TRIPPLANNER_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include "trip.h"
#include "hashmap.h"
#include "stadiumgraph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TripPlanner; }
QT_END_NAMESPACE

class TripPlanner : public QDialog
{
    Q_OBJECT

public:
    explicit TripPlanner(const HashMap<QString, StadiumInfo>& stadiumMap, StadiumGraph* stadiumGraph, QWidget *parent = nullptr);
    ~TripPlanner();
    void refreshStadiumLists();

private slots:
    void on_dijkstraButton_clicked();
    void on_aStarButton_clicked();
    void on_greedyButton_clicked();
    void on_mstButton_clicked();
    void on_dfsButton_clicked();
    void on_bfsButton_clicked();
    void on_addStopButton_clicked();
    void on_removeStopButton_clicked();
    void on_addSouvenirButton_clicked();
    void on_clearTripButton_clicked();
    void on_sortButton_clicked();
    void on_filterLeagueCombo_currentIndexChanged(const QString &league);
    void updateTripInfo();
    void updateSouvenirTableForSelectedStadium();
    void on_dodgerToAnyButton_clicked();
    void on_customOrderTripButton_clicked();
    void on_visitAllMarlinsButton_clicked();
    void on_dreamVacationButton_clicked();
    void on_dfsOracleButton_clicked();
    void on_bfsTargetButton_clicked();
    void on_startingStadiumCombo_currentIndexChanged(const QString &stadium);
    void on_planTripButton_clicked();
    void updateAlgorithmUIVisibility();
    void on_removeSouvenirButton_clicked();

private:
    Ui::TripPlanner *ui;
    Trip currentTrip;
    const HashMap<QString, StadiumInfo>& stadiumMap;
    StadiumGraph* stadiumGraph;
    QMap<QString, QVector<QPair<QString, int>>> souvenirCart; // stadium -> (souvenir, qty)
    void setupUi();
    void updateStopList();
    void updateTotalCost();
    void updateTotalDistance();
    void updateOverallSouvenirSummary();
    QString findTeamNameByStadium(const QString& normalizedStadium) const;
};

#endif // TRIPPLANNER_H 