#include "mainwindow.h"
#include "stadiumgraph.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StadiumGraph* stadiumGraph = new StadiumGraph();
    stadiumGraph->loadFromCSV("MLB Information.csv"); // Adjust path if needed

    MainWindow w;
    w.setStadiumGraph(stadiumGraph);
    w.show();
    return a.exec();
} 