#include "adminmanager.h"
#include <QMessageBox>
#include <QDebug>

AdminManager::AdminManager(QObject *parent)
    : QObject(parent)
    , traditionalSouvenirsFile("traditional_souvenirs.txt")
{
    loadTraditionalSouvenirs();
}

AdminManager::~AdminManager()
{
    saveTraditionalSouvenirs();
}

bool AdminManager::addNewStadium(const QString &stadiumName, const QString &teamName, const QString &souvenirFile)
{
    // Read the souvenir file and add the stadium and its souvenirs to the database
    QFile file(souvenirFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Could not open souvenir file");
        return false;
    }

    QTextStream in(&file);
    QMap<QString, double> souvenirs;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts.size() == 2) {
            QString name = parts[0].trimmed();
            double price = parts[1].toDouble();
            souvenirs[name] = price;
        }
    }

    // TODO: Add the stadium and its souvenirs to the database
    // This would involve updating the MLB Information.csv file
    return true;
}

bool AdminManager::updateStadiumSouvenirs(const QString &stadiumName, const QMap<QString, double> &souvenirs)
{
    // TODO: Update the souvenirs for the specified stadium in the database
    return true;
}

bool AdminManager::addTraditionalSouvenir(const QString &name, double price)
{
    if (traditionalSouvenirs.contains(name)) {
        QMessageBox::warning(nullptr, "Warning", "Souvenir already exists");
        return false;
    }

    traditionalSouvenirs[name] = price;
    return saveTraditionalSouvenirs();
}

bool AdminManager::updateTraditionalSouvenirPrice(const QString &name, double newPrice)
{
    if (!traditionalSouvenirs.contains(name)) {
        QMessageBox::warning(nullptr, "Warning", "Souvenir not found");
        return false;
    }

    traditionalSouvenirs[name] = newPrice;
    return saveTraditionalSouvenirs();
}

bool AdminManager::deleteTraditionalSouvenir(const QString &name)
{
    if (!traditionalSouvenirs.contains(name)) {
        QMessageBox::warning(nullptr, "Warning", "Souvenir not found");
        return false;
    }

    traditionalSouvenirs.remove(name);
    return saveTraditionalSouvenirs();
}

QMap<QString, double> AdminManager::getTraditionalSouvenirs() const
{
    return traditionalSouvenirs;
}

bool AdminManager::loadTraditionalSouvenirs()
{
    QFile file(traditionalSouvenirsFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    traditionalSouvenirs.clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts.size() == 2) {
            QString name = parts[0].trimmed();
            double price = parts[1].toDouble();
            traditionalSouvenirs[name] = price;
        }
    }

    return true;
}

bool AdminManager::saveTraditionalSouvenirs()
{
    QFile file(traditionalSouvenirsFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Could not save traditional souvenirs");
        return false;
    }

    QTextStream out(&file);
    for (auto it = traditionalSouvenirs.begin(); it != traditionalSouvenirs.end(); ++it) {
        out << it.key() << "," << it.value() << "\n";
    }

    return true;
} 