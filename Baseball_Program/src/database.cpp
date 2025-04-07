#include "database.h"
#include <QTextStream>
#include <QFile>

Database::Database()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("baseball.db");
}

Database::~Database()
{
    if (db.isOpen())
        db.close();
}

bool Database::initialize()
{
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }

    createTables();
    insertInitialData();
    return true;
}

void Database::createTables()
{
    QSqlQuery query;
    query.exec("DROP TABLE IF EXISTS Teams");
    query.exec(
        "CREATE TABLE Teams ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "team_name TEXT NOT NULL,"
        "stadium_name TEXT NOT NULL,"
        "seating_capacity INTEGER,"
        "location TEXT,"
        "playing_surface TEXT,"
        "league TEXT,"
        "date_opened DATE,"
        "distance_to_center INTEGER,"
        "ballpark_typology TEXT,"
        "roof_type TEXT"
        ")"
    );
}

void Database::insertInitialData()
{
    QSqlQuery query;
    // Sample data - you would want to add all MLB teams here
    query.prepare(
        "INSERT INTO Teams (team_name, stadium_name, seating_capacity, location, "
        "playing_surface, league, date_opened, distance_to_center, ballpark_typology, roof_type) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );

    // Add some sample teams (you should add all MLB teams)
    QStringList teams = {
        "New York Yankees|Yankee Stadium|47309|Bronx, NY|Grass|American|2009-04-16|408|Retro-Modern|Open",
        "Boston Red Sox|Fenway Park|37755|Boston, MA|Grass|American|1912-04-20|390|Jewel Box|Open",
        "Los Angeles Dodgers|Dodger Stadium|56000|Los Angeles, CA|Grass|National|1962-04-10|395|Modern|Open",
        // Add more teams here
    };

    for (const QString &team : teams) {
        QStringList data = team.split("|");
        query.addBindValue(data[0]); // team_name
        query.addBindValue(data[1]); // stadium_name
        query.addBindValue(data[2].toInt()); // seating_capacity
        query.addBindValue(data[3]); // location
        query.addBindValue(data[4]); // playing_surface
        query.addBindValue(data[5]); // league
        query.addBindValue(data[6]); // date_opened
        query.addBindValue(data[7].toInt()); // distance_to_center
        query.addBindValue(data[8]); // ballpark_typology
        query.addBindValue(data[9]); // roof_type
        query.exec();
    }
}

QSqlQuery Database::getTeamInfo(const QString &teamName)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM Teams WHERE team_name = ?");
    query.addBindValue(teamName);
    query.exec();
    return query;
}

QSqlQuery Database::getAllTeamsSortedByTeamName()
{
    QSqlQuery query;
    query.exec("SELECT team_name, stadium_name FROM Teams ORDER BY team_name");
    return query;
}

QSqlQuery Database::getAllTeamsSortedByStadiumName()
{
    QSqlQuery query;
    query.exec("SELECT team_name, stadium_name FROM Teams ORDER BY stadium_name");
    return query;
}

QSqlQuery Database::getAmericanLeagueTeams()
{
    QSqlQuery query;
    query.exec("SELECT team_name, stadium_name FROM Teams WHERE league = 'American' ORDER BY team_name");
    return query;
}

QSqlQuery Database::getNationalLeagueTeams()
{
    QSqlQuery query;
    query.exec("SELECT team_name, stadium_name FROM Teams WHERE league = 'National' ORDER BY stadium_name");
    return query;
}

QSqlQuery Database::getTeamsByTypology()
{
    QSqlQuery query;
    query.exec("SELECT stadium_name, team_name, ballpark_typology FROM Teams ORDER BY ballpark_typology");
    return query;
}

QSqlQuery Database::getOpenRoofTeams()
{
    QSqlQuery query;
    query.exec("SELECT team_name FROM Teams WHERE roof_type = 'Open' ORDER BY team_name");
    return query;
}

QSqlQuery Database::getTeamsByDateOpened()
{
    QSqlQuery query;
    query.exec("SELECT stadium_name, team_name, date_opened FROM Teams ORDER BY date_opened");
    return query;
}

QSqlQuery Database::getTeamsByCapacity()
{
    QSqlQuery query;
    query.exec("SELECT stadium_name, team_name, seating_capacity FROM Teams ORDER BY seating_capacity");
    return query;
}

QSqlQuery Database::getTeamsWithGreatestCenterField()
{
    QSqlQuery query;
    query.exec(
        "SELECT stadium_name, team_name, distance_to_center FROM Teams "
        "WHERE distance_to_center = (SELECT MAX(distance_to_center) FROM Teams)"
    );
    return query;
}

QSqlQuery Database::getTeamsWithSmallestCenterField()
{
    QSqlQuery query;
    query.exec(
        "SELECT stadium_name, team_name, distance_to_center FROM Teams "
        "WHERE distance_to_center = (SELECT MIN(distance_to_center) FROM Teams)"
    );
    return query;
}

bool Database::importFromCSV(const QString &csvFilePath)
{
    QFile file(csvFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening CSV file:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    // Skip header row
    in.readLine();

    QSqlQuery query;
    query.prepare(
        "INSERT INTO Teams (team_name, stadium_name, seating_capacity, location, "
        "playing_surface, league, date_opened, distance_to_center, ballpark_typology, roof_type) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
    );

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith(",")) continue; // Skip empty lines

        // Remove any trailing commas
        while (line.endsWith(',')) {
            line.chop(1);
        }

        QStringList fields = line.split(",");
        
        // Clean up each field
        for (int i = 0; i < fields.size(); i++) {
            fields[i] = fields[i].trimmed();
            // Remove quotes if present
            if (fields[i].startsWith('"') && fields[i].endsWith('"')) {
                fields[i] = fields[i].mid(1, fields[i].length() - 2);
            }
        }

        // Ensure we have enough fields
        if (fields.size() < 10) {
            qDebug() << "Skipping line - not enough fields:" << line;
            continue;
        }

        // Clean up specific fields
        QString teamName = fields[0].trimmed();
        QString stadiumName = fields[1].trimmed();
        
        // Clean up seating capacity (remove commas and quotes)
        QString capacityStr = fields[2].replace(",", "").replace("\"", "");
        int seatingCapacity = capacityStr.toInt();
        
        QString location = fields[3].trimmed();
        QString playingSurface = fields[4].trimmed();
        QString league = fields[5].trimmed();
        
        // Clean up date (take just the year if it's a full date)
        QString dateStr = fields[6].trimmed();
        if (dateStr.contains("-")) {
            dateStr = dateStr.split("-")[0] + "-01-01"; // Use first day of the year
        }
        
        // Clean up distance (extract just the number)
        QString distanceStr = fields[7].trimmed();
        int distance = 0;
        if (distanceStr.contains("feet")) {
            distanceStr = distanceStr.split("feet")[0].trimmed();
            distance = distanceStr.toInt();
        }
        
        QString typology = fields[8].trimmed();
        QString roofType = fields[9].trimmed();

        // Add the cleaned values to the query
        query.addBindValue(teamName);
        query.addBindValue(stadiumName);
        query.addBindValue(seatingCapacity);
        query.addBindValue(location);
        query.addBindValue(playingSurface);
        query.addBindValue(league);
        query.addBindValue(dateStr);
        query.addBindValue(distance);
        query.addBindValue(typology);
        query.addBindValue(roofType);

        if (!query.exec()) {
            qDebug() << "Error inserting data:" << query.lastError().text();
            qDebug() << "Problematic line:" << line;
            return false;
        }
    }

    file.close();
    return true;
} 