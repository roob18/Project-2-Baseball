#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/style.css");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString style = file.readAll();
        a.setStyleSheet(style);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
