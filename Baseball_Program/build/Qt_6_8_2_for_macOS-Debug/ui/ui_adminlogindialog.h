/********************************************************************************
** Form generated from reading UI file 'adminlogindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMINLOGINDIALOG_H
#define UI_ADMINLOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AdminLoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QSpacerItem *verticalSpacer;
    QFormLayout *formLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *loginButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *AdminLoginDialog)
    {
        if (AdminLoginDialog->objectName().isEmpty())
            AdminLoginDialog->setObjectName("AdminLoginDialog");
        AdminLoginDialog->resize(400, 200);
        verticalLayout = new QVBoxLayout(AdminLoginDialog);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(AdminLoginDialog);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        usernameLabel = new QLabel(AdminLoginDialog);
        usernameLabel->setObjectName("usernameLabel");

        formLayout->setWidget(0, QFormLayout::LabelRole, usernameLabel);

        usernameLineEdit = new QLineEdit(AdminLoginDialog);
        usernameLineEdit->setObjectName("usernameLineEdit");

        formLayout->setWidget(0, QFormLayout::FieldRole, usernameLineEdit);

        passwordLabel = new QLabel(AdminLoginDialog);
        passwordLabel->setObjectName("passwordLabel");

        formLayout->setWidget(1, QFormLayout::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(AdminLoginDialog);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(1, QFormLayout::FieldRole, passwordLineEdit);


        verticalLayout->addLayout(formLayout);

        verticalSpacer_2 = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        loginButton = new QPushButton(AdminLoginDialog);
        loginButton->setObjectName("loginButton");

        horizontalLayout->addWidget(loginButton);

        cancelButton = new QPushButton(AdminLoginDialog);
        cancelButton->setObjectName("cancelButton");

        horizontalLayout->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(AdminLoginDialog);

        QMetaObject::connectSlotsByName(AdminLoginDialog);
    } // setupUi

    void retranslateUi(QDialog *AdminLoginDialog)
    {
        AdminLoginDialog->setWindowTitle(QCoreApplication::translate("AdminLoginDialog", "Admin Login", nullptr));
        titleLabel->setText(QCoreApplication::translate("AdminLoginDialog", "Administrator Login", nullptr));
        usernameLabel->setText(QCoreApplication::translate("AdminLoginDialog", "Username:", nullptr));
        passwordLabel->setText(QCoreApplication::translate("AdminLoginDialog", "Password:", nullptr));
        loginButton->setText(QCoreApplication::translate("AdminLoginDialog", "Login", nullptr));
        cancelButton->setText(QCoreApplication::translate("AdminLoginDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminLoginDialog: public Ui_AdminLoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINLOGINDIALOG_H
