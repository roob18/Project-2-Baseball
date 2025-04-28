#include "shortesttripdialog.h"
#include "ui_shortesttripdialog.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMessageBox>

ShortestTripDialog::ShortestTripDialog(const HashMap<QString, StadiumInfo>& stadiumMap, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ShortestTripDialog)
    , stadiumMap(stadiumMap)
{
    setupUi();
}

ShortestTripDialog::~ShortestTripDialog()
{
    delete ui;
}

void ShortestTripDialog::setupUi()
{
    ui->setupUi(this);
    setWindowTitle("Plan Shortest Trip from Dodger Stadium");

    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create scroll area for stadium checkboxes
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // Add stadium checkboxes
    QVector<QPair<QString, StadiumInfo>> entries = stadiumMap.getAllEntries();
    for (const auto& entry : entries) {
        if (entry.first != "Los Angeles Dodgers") { // Skip Dodger Stadium as it's the starting point
            QCheckBox *checkbox = new QCheckBox(entry.first, scrollContent);
            scrollLayout->addWidget(checkbox);
        }
    }

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Add buttons
    QPushButton *selectAllButton = new QPushButton("Select All", this);
    QPushButton *deselectAllButton = new QPushButton("Deselect All", this);
    QPushButton *planTripButton = new QPushButton("Plan Trip", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(selectAllButton);
    buttonLayout->addWidget(deselectAllButton);
    buttonLayout->addWidget(planTripButton);
    mainLayout->addLayout(buttonLayout);

    // Add route display
    QLabel *routeLabel = new QLabel("Route:", this);
    QListWidget *routeList = new QListWidget(this);
    QLabel *distanceLabel = new QLabel("Total Distance: 0 miles", this);

    mainLayout->addWidget(routeLabel);
    mainLayout->addWidget(routeList);
    mainLayout->addWidget(distanceLabel);

    // Connect signals and slots
    connect(selectAllButton, &QPushButton::clicked, this, &ShortestTripDialog::on_selectAllButton_clicked);
    connect(deselectAllButton, &QPushButton::clicked, this, &ShortestTripDialog::on_deselectAllButton_clicked);
    connect(planTripButton, &QPushButton::clicked, this, &ShortestTripDialog::on_planTripButton_clicked);

    setLayout(mainLayout);
}

void ShortestTripDialog::on_planTripButton_clicked()
{
    QVector<QString> selectedStadiums;
    
    // Get all checkboxes in the scroll area
    QWidget *scrollContent = ui->scrollArea->widget();
    QList<QCheckBox*> checkboxes = scrollContent->findChildren<QCheckBox*>();
    
    // Collect selected stadiums
    for (QCheckBox *checkbox : checkboxes) {
        if (checkbox->isChecked()) {
            selectedStadiums.append(checkbox->text());
        }
    }
    
    if (selectedStadiums.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select at least one stadium to visit.");
        return;
    }
    
    // Find shortest path starting from Dodger Stadium
    QVector<QString> path = trip.findShortestPath("Los Angeles Dodgers", selectedStadiums, stadiumMap);
    
    // Update the display
    updateRouteDisplay(path);
}

void ShortestTripDialog::on_selectAllButton_clicked()
{
    QWidget *scrollContent = ui->scrollArea->widget();
    QList<QCheckBox*> checkboxes = scrollContent->findChildren<QCheckBox*>();
    
    for (QCheckBox *checkbox : checkboxes) {
        checkbox->setChecked(true);
    }
}

void ShortestTripDialog::on_deselectAllButton_clicked()
{
    QWidget *scrollContent = ui->scrollArea->widget();
    QList<QCheckBox*> checkboxes = scrollContent->findChildren<QCheckBox*>();
    
    for (QCheckBox *checkbox : checkboxes) {
        checkbox->setChecked(false);
    }
}

void ShortestTripDialog::updateRouteDisplay(const QVector<QString>& path)
{
    ui->routeList->clear();
    
    // Add Dodger Stadium as the starting point
    ui->routeList->addItem("1. Los Angeles Dodgers (Starting Point)");
    
    // Add the rest of the route
    for (int i = 0; i < path.size(); ++i) {
        ui->routeList->addItem(QString("%1. %2").arg(i + 2).arg(path[i]));
    }
    
    // Update total distance
    double totalDistance = calculateTotalDistance(path);
    ui->distanceLabel->setText(QString("Total Distance: %1 miles").arg(totalDistance, 0, 'f', 1));
}

double ShortestTripDialog::calculateTotalDistance(const QVector<QString>& path)
{
    return trip.getPathDistance(path, stadiumMap);
} 