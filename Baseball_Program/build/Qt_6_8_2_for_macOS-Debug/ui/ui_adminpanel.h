/********************************************************************************
** Form generated from reading UI file 'adminpanel.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMINPANEL_H
#define UI_ADMINPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AdminPanel
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QTabWidget *tabWidget;
    QWidget *stadiumTab;
    QVBoxLayout *verticalLayout_2;
    QPushButton *importStadiumButton;
    QTableWidget *stadiumTable;
    QWidget *souvenirTab;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *teamComboBox;
    QTableWidget *souvenirTable;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *addSouvenirButton;
    QPushButton *editSouvenirButton;
    QPushButton *deleteSouvenirButton;
    QPushButton *closeButton;

    void setupUi(QDialog *AdminPanel)
    {
        if (AdminPanel->objectName().isEmpty())
            AdminPanel->setObjectName("AdminPanel");
        AdminPanel->resize(800, 600);
        verticalLayout = new QVBoxLayout(AdminPanel);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(AdminPanel);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        tabWidget = new QTabWidget(AdminPanel);
        tabWidget->setObjectName("tabWidget");
        stadiumTab = new QWidget();
        stadiumTab->setObjectName("stadiumTab");
        verticalLayout_2 = new QVBoxLayout(stadiumTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        importStadiumButton = new QPushButton(stadiumTab);
        importStadiumButton->setObjectName("importStadiumButton");

        verticalLayout_2->addWidget(importStadiumButton);

        stadiumTable = new QTableWidget(stadiumTab);
        stadiumTable->setObjectName("stadiumTable");

        verticalLayout_2->addWidget(stadiumTable);

        tabWidget->addTab(stadiumTab, QString());
        souvenirTab = new QWidget();
        souvenirTab->setObjectName("souvenirTab");
        verticalLayout_3 = new QVBoxLayout(souvenirTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(souvenirTab);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        teamComboBox = new QComboBox(souvenirTab);
        teamComboBox->setObjectName("teamComboBox");

        horizontalLayout->addWidget(teamComboBox);


        verticalLayout_3->addLayout(horizontalLayout);

        souvenirTable = new QTableWidget(souvenirTab);
        souvenirTable->setObjectName("souvenirTable");

        verticalLayout_3->addWidget(souvenirTable);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        addSouvenirButton = new QPushButton(souvenirTab);
        addSouvenirButton->setObjectName("addSouvenirButton");

        horizontalLayout_2->addWidget(addSouvenirButton);

        editSouvenirButton = new QPushButton(souvenirTab);
        editSouvenirButton->setObjectName("editSouvenirButton");

        horizontalLayout_2->addWidget(editSouvenirButton);

        deleteSouvenirButton = new QPushButton(souvenirTab);
        deleteSouvenirButton->setObjectName("deleteSouvenirButton");

        horizontalLayout_2->addWidget(deleteSouvenirButton);


        verticalLayout_3->addLayout(horizontalLayout_2);

        tabWidget->addTab(souvenirTab, QString());

        verticalLayout->addWidget(tabWidget);

        closeButton = new QPushButton(AdminPanel);
        closeButton->setObjectName("closeButton");

        verticalLayout->addWidget(closeButton);


        retranslateUi(AdminPanel);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AdminPanel);
    } // setupUi

    void retranslateUi(QDialog *AdminPanel)
    {
        AdminPanel->setWindowTitle(QCoreApplication::translate("AdminPanel", "Admin Panel", nullptr));
        titleLabel->setText(QCoreApplication::translate("AdminPanel", "Baseball Stadium Administrator Panel", nullptr));
        importStadiumButton->setText(QCoreApplication::translate("AdminPanel", "Import Stadium Data", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(stadiumTab), QCoreApplication::translate("AdminPanel", "Stadium Management", nullptr));
        label->setText(QCoreApplication::translate("AdminPanel", "Select Team:", nullptr));
        addSouvenirButton->setText(QCoreApplication::translate("AdminPanel", "Add Souvenir", nullptr));
        editSouvenirButton->setText(QCoreApplication::translate("AdminPanel", "Edit Selected", nullptr));
        deleteSouvenirButton->setText(QCoreApplication::translate("AdminPanel", "Delete Selected", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(souvenirTab), QCoreApplication::translate("AdminPanel", "Souvenir Management", nullptr));
        closeButton->setText(QCoreApplication::translate("AdminPanel", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminPanel: public Ui_AdminPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINPANEL_H
