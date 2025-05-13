#include "mainwindow.h"
#include "stadiumgraph.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    StadiumGraph* stadiumGraph = new StadiumGraph();
    stadiumGraph->loadFromDatabase(w.getDatabase());
    w.setStadiumGraph(stadiumGraph);
    w.show();
    return a.exec();
} 