/********************************************************************************
** Form generated from reading UI file 'souvenirdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SOUVENIRDIALOG_H
#define UI_SOUVENIRDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SouvenirDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTableView *souvenirTableView;
    QHBoxLayout *horizontalLayout;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *closeButton;

    void setupUi(QDialog *SouvenirDialog)
    {
        if (SouvenirDialog->objectName().isEmpty())
            SouvenirDialog->setObjectName("SouvenirDialog");
        SouvenirDialog->resize(600, 400);
        verticalLayout = new QVBoxLayout(SouvenirDialog);
        verticalLayout->setObjectName("verticalLayout");
        souvenirTableView = new QTableView(SouvenirDialog);
        souvenirTableView->setObjectName("souvenirTableView");
        souvenirTableView->setSelectionMode(QAbstractItemView::SingleSelection);
        souvenirTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(souvenirTableView);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        addButton = new QPushButton(SouvenirDialog);
        addButton->setObjectName("addButton");

        horizontalLayout->addWidget(addButton);

        editButton = new QPushButton(SouvenirDialog);
        editButton->setObjectName("editButton");

        horizontalLayout->addWidget(editButton);

        deleteButton = new QPushButton(SouvenirDialog);
        deleteButton->setObjectName("deleteButton");

        horizontalLayout->addWidget(deleteButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        closeButton = new QPushButton(SouvenirDialog);
        closeButton->setObjectName("closeButton");

        horizontalLayout->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SouvenirDialog);

        QMetaObject::connectSlotsByName(SouvenirDialog);
    } // setupUi

    void retranslateUi(QDialog *SouvenirDialog)
    {
        SouvenirDialog->setWindowTitle(QCoreApplication::translate("SouvenirDialog", "Manage Souvenirs", nullptr));
        addButton->setText(QCoreApplication::translate("SouvenirDialog", "Add Souvenir", nullptr));
        editButton->setText(QCoreApplication::translate("SouvenirDialog", "Edit Souvenir", nullptr));
        deleteButton->setText(QCoreApplication::translate("SouvenirDialog", "Delete Souvenir", nullptr));
        closeButton->setText(QCoreApplication::translate("SouvenirDialog", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SouvenirDialog: public Ui_SouvenirDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOUVENIRDIALOG_H
