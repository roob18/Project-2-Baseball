/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *buttonLayout;
    QComboBox *teamComboBox;
    QPushButton *teamInfoButton;
    QPushButton *sortByTeamButton;
    QPushButton *sortByStadiumButton;
    QPushButton *americanLeagueButton;
    QPushButton *nationalLeagueButton;
    QPushButton *typologyButton;
    QPushButton *openRoofButton;
    QPushButton *dateOpenedButton;
    QPushButton *capacityButton;
    QPushButton *maxCenterFieldButton;
    QPushButton *minCenterFieldButton;
    QPushButton *importCSVButton;
    QPushButton *viewSouvenirsButton;
    QPushButton *adminLoginButton;
    QSpacerItem *verticalSpacer;
    QTableWidget *resultsTable;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 800);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        buttonLayout = new QVBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        teamComboBox = new QComboBox(centralwidget);
        teamComboBox->setObjectName("teamComboBox");
        teamComboBox->setMinimumWidth(200);

        buttonLayout->addWidget(teamComboBox);

        teamInfoButton = new QPushButton(centralwidget);
        teamInfoButton->setObjectName("teamInfoButton");

        buttonLayout->addWidget(teamInfoButton);

        sortByTeamButton = new QPushButton(centralwidget);
        sortByTeamButton->setObjectName("sortByTeamButton");

        buttonLayout->addWidget(sortByTeamButton);

        sortByStadiumButton = new QPushButton(centralwidget);
        sortByStadiumButton->setObjectName("sortByStadiumButton");

        buttonLayout->addWidget(sortByStadiumButton);

        americanLeagueButton = new QPushButton(centralwidget);
        americanLeagueButton->setObjectName("americanLeagueButton");

        buttonLayout->addWidget(americanLeagueButton);

        nationalLeagueButton = new QPushButton(centralwidget);
        nationalLeagueButton->setObjectName("nationalLeagueButton");

        buttonLayout->addWidget(nationalLeagueButton);

        typologyButton = new QPushButton(centralwidget);
        typologyButton->setObjectName("typologyButton");

        buttonLayout->addWidget(typologyButton);

        openRoofButton = new QPushButton(centralwidget);
        openRoofButton->setObjectName("openRoofButton");

        buttonLayout->addWidget(openRoofButton);

        dateOpenedButton = new QPushButton(centralwidget);
        dateOpenedButton->setObjectName("dateOpenedButton");

        buttonLayout->addWidget(dateOpenedButton);

        capacityButton = new QPushButton(centralwidget);
        capacityButton->setObjectName("capacityButton");

        buttonLayout->addWidget(capacityButton);

        maxCenterFieldButton = new QPushButton(centralwidget);
        maxCenterFieldButton->setObjectName("maxCenterFieldButton");

        buttonLayout->addWidget(maxCenterFieldButton);

        minCenterFieldButton = new QPushButton(centralwidget);
        minCenterFieldButton->setObjectName("minCenterFieldButton");

        buttonLayout->addWidget(minCenterFieldButton);

        importCSVButton = new QPushButton(centralwidget);
        importCSVButton->setObjectName("importCSVButton");

        buttonLayout->addWidget(importCSVButton);

        viewSouvenirsButton = new QPushButton(centralwidget);
        viewSouvenirsButton->setObjectName("viewSouvenirsButton");
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("view-list-details")));
        viewSouvenirsButton->setIcon(icon);

        buttonLayout->addWidget(viewSouvenirsButton);

        adminLoginButton = new QPushButton(centralwidget);
        adminLoginButton->setObjectName("adminLoginButton");
        QIcon icon1(QIcon::fromTheme(QString::fromUtf8("dialog-password")));
        adminLoginButton->setIcon(icon1);

        buttonLayout->addWidget(adminLoginButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        buttonLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(buttonLayout);

        resultsTable = new QTableWidget(centralwidget);
        resultsTable->setObjectName("resultsTable");
        resultsTable->setMinimumWidth(700);

        horizontalLayout->addWidget(resultsTable);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1000, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MLB Stadium Information", nullptr));
        teamInfoButton->setText(QCoreApplication::translate("MainWindow", "Show Team Info", nullptr));
        sortByTeamButton->setText(QCoreApplication::translate("MainWindow", "Sort by Team Name", nullptr));
        sortByStadiumButton->setText(QCoreApplication::translate("MainWindow", "Sort by Stadium Name", nullptr));
        americanLeagueButton->setText(QCoreApplication::translate("MainWindow", "American League Teams", nullptr));
        nationalLeagueButton->setText(QCoreApplication::translate("MainWindow", "National League Teams", nullptr));
        typologyButton->setText(QCoreApplication::translate("MainWindow", "Sort by Park Typology", nullptr));
        openRoofButton->setText(QCoreApplication::translate("MainWindow", "Teams with Open Roof", nullptr));
        dateOpenedButton->setText(QCoreApplication::translate("MainWindow", "Sort by Date Opened", nullptr));
        capacityButton->setText(QCoreApplication::translate("MainWindow", "Sort by Capacity", nullptr));
        maxCenterFieldButton->setText(QCoreApplication::translate("MainWindow", "Greatest Center Field", nullptr));
        minCenterFieldButton->setText(QCoreApplication::translate("MainWindow", "Smallest Center Field", nullptr));
        importCSVButton->setText(QCoreApplication::translate("MainWindow", "Import CSV Data", nullptr));
        viewSouvenirsButton->setText(QCoreApplication::translate("MainWindow", "View Team Souvenirs", nullptr));
        adminLoginButton->setText(QCoreApplication::translate("MainWindow", "Admin Login", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
