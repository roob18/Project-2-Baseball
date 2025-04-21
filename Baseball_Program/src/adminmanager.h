#ifndef ADMINMANAGER_H
#define ADMINMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QFile>
#include <QTextStream>

class AdminManager : public QObject
{
    Q_OBJECT

public:
    explicit AdminManager(QObject *parent = nullptr);
    ~AdminManager();

    // Stadium management
    bool addNewStadium(const QString &stadiumName, const QString &teamName, const QString &souvenirFile);
    bool updateStadiumSouvenirs(const QString &stadiumName, const QMap<QString, double> &souvenirs);

    // Traditional souvenir management
    bool addTraditionalSouvenir(const QString &name, double price);
    bool updateTraditionalSouvenirPrice(const QString &name, double newPrice);
    bool deleteTraditionalSouvenir(const QString &name);
    QMap<QString, double> getTraditionalSouvenirs() const;

private:
    QMap<QString, double> traditionalSouvenirs;
    QString traditionalSouvenirsFile;

    bool loadTraditionalSouvenirs();
    bool saveTraditionalSouvenirs();
};

#endif // ADMINMANAGER_H 