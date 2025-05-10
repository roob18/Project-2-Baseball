#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "adminmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_adminLoginButton_clicked();
    void on_addStadiumButton_clicked();
    void on_updateSouvenirPriceButton_clicked();
    void on_addTraditionalSouvenirButton_clicked();
    void on_deleteTraditionalSouvenirButton_clicked();

private:
    Ui::MainWindow *ui;
    AdminManager *adminManager;
    bool isAdminLoggedIn;

    void setupAdminUI();
    void showAdminOnlyUI(bool show);
};
#endif // MAINWINDOW_H
