/********************************************************************************
** Form generated from reading UI file 'tripplanner.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIPPLANNER_H
#define UI_TRIPPLANNER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TripPlanner
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *filterLeagueCombo;
    QSpacerItem *horizontalSpacer;
    QListWidget *stopList;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *addStopButton;
    QPushButton *removeStopButton;
    QPushButton *addSouvenirButton;
    QPushButton *sortButton;
    QPushButton *clearTripButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *totalDistanceLabel;
    QSpacerItem *horizontalSpacer_2;
    QLabel *totalCostLabel;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TripPlanner)
    {
        if (TripPlanner->objectName().isEmpty())
            TripPlanner->setObjectName("TripPlanner");
        TripPlanner->resize(600, 500);
        verticalLayout = new QVBoxLayout(TripPlanner);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(TripPlanner);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        filterLeagueCombo = new QComboBox(TripPlanner);
        filterLeagueCombo->setObjectName("filterLeagueCombo");

        horizontalLayout->addWidget(filterLeagueCombo);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        stopList = new QListWidget(TripPlanner);
        stopList->setObjectName("stopList");
        stopList->setSelectionMode(QAbstractItemView::SingleSelection);

        verticalLayout->addWidget(stopList);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        addStopButton = new QPushButton(TripPlanner);
        addStopButton->setObjectName("addStopButton");

        horizontalLayout_2->addWidget(addStopButton);

        removeStopButton = new QPushButton(TripPlanner);
        removeStopButton->setObjectName("removeStopButton");

        horizontalLayout_2->addWidget(removeStopButton);

        addSouvenirButton = new QPushButton(TripPlanner);
        addSouvenirButton->setObjectName("addSouvenirButton");

        horizontalLayout_2->addWidget(addSouvenirButton);

        sortButton = new QPushButton(TripPlanner);
        sortButton->setObjectName("sortButton");

        horizontalLayout_2->addWidget(sortButton);

        clearTripButton = new QPushButton(TripPlanner);
        clearTripButton->setObjectName("clearTripButton");

        horizontalLayout_2->addWidget(clearTripButton);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        totalDistanceLabel = new QLabel(TripPlanner);
        totalDistanceLabel->setObjectName("totalDistanceLabel");

        horizontalLayout_3->addWidget(totalDistanceLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        totalCostLabel = new QLabel(TripPlanner);
        totalCostLabel->setObjectName("totalCostLabel");

        horizontalLayout_3->addWidget(totalCostLabel);


        verticalLayout->addLayout(horizontalLayout_3);

        buttonBox = new QDialogButtonBox(TripPlanner);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(TripPlanner);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, TripPlanner, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, TripPlanner, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TripPlanner);
    } // setupUi

    void retranslateUi(QDialog *TripPlanner)
    {
        TripPlanner->setWindowTitle(QCoreApplication::translate("TripPlanner", "Trip Planner", nullptr));
        label->setText(QCoreApplication::translate("TripPlanner", "Filter by League:", nullptr));
        addStopButton->setText(QCoreApplication::translate("TripPlanner", "Add Stop", nullptr));
        removeStopButton->setText(QCoreApplication::translate("TripPlanner", "Remove Stop", nullptr));
        addSouvenirButton->setText(QCoreApplication::translate("TripPlanner", "Add Souvenir", nullptr));
        sortButton->setText(QCoreApplication::translate("TripPlanner", "Sort by Team", nullptr));
        clearTripButton->setText(QCoreApplication::translate("TripPlanner", "Clear Trip", nullptr));
        totalDistanceLabel->setText(QCoreApplication::translate("TripPlanner", "Total Distance: 0 miles", nullptr));
        totalCostLabel->setText(QCoreApplication::translate("TripPlanner", "Total Cost: $0.00", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TripPlanner: public Ui_TripPlanner {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIPPLANNER_H
