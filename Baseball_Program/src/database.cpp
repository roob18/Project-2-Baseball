#include "database.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStringConverter>
#include <QSqlError>

Database::Database(QObject *parent)
    : QObject(parent)
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
    // First, try to find the exact team name
    query.prepare("SELECT * FROM Teams WHERE team_name = ? COLLATE NOCASE");
    query.addBindValue(teamName);
    query.exec();
    
    // If no exact match, try a partial match
    if (!query.next()) {
        query.prepare("SELECT * FROM Teams WHERE team_name LIKE ? COLLATE NOCASE");
        query.addBindValue("%" + teamName + "%");
        query.exec();
    } else {
        // Reset the query position if we found an exact match
        query.previous();
    }
    
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
        "WHERE distance_to_center > 0 "  // Exclude invalid/zero distances
        "ORDER BY distance_to_center DESC"
    );
    return query;
}

QSqlQuery Database::getTeamsWithSmallestCenterField()
{
    QSqlQuery query;
    query.exec(
        "SELECT stadium_name, team_name, distance_to_center FROM Teams "
        "WHERE distance_to_center > 0 "  // Exclude invalid/zero distances
        "ORDER BY distance_to_center ASC"
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

    // Clear existing data
    QSqlQuery clearQuery;
    clearQuery.exec("DELETE FROM Teams");

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    // Skip header row
    QString header = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line == ",,,,,,,,,,,,") continue;  // Skip empty lines or lines with just commas

        // Handle fields that contain commas within quotes
        QStringList fields;
        QRegularExpression re("(\"([^\"]*)\"|([^,]+))");
        QRegularExpressionMatchIterator i = re.globalMatch(line);
        
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString field = match.captured(2);
            if (field.isEmpty()) {
                field = match.captured(1);
            }
            field = field.trimmed();
            if (field.startsWith('\"') && field.endsWith('\"')) {
                field = field.mid(1, field.length() - 2);
            }
            if (!field.isEmpty()) {  // Only add non-empty fields
                fields.append(field);
            }
        }

        // Skip if we don't have enough fields
        if (fields.size() < 10) {  // We expect at least 10 fields
            continue;
        }

        // Clean up the distance field - extract just the number
        QString distanceStr = fields[7];  // Distance is in the 8th field
        QRegularExpression numRegex("\\d+");
        QRegularExpressionMatch match = numRegex.match(distanceStr);
        int distance = match.hasMatch() ? match.captured(0).toInt() : 0;

        QSqlQuery query;
        query.prepare(
            "INSERT INTO Teams (team_name, stadium_name, seating_capacity, location, "
            "playing_surface, league, date_opened, distance_to_center, ballpark_typology, roof_type) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

        // Use correct indices (0-9 since we have 10 fields)
        query.addBindValue(fields[0].trimmed());  // team_name
        query.addBindValue(fields[1].trimmed());  // stadium_name
        query.addBindValue(fields[2].remove(',').toInt());  // seating_capacity
        query.addBindValue(fields[3].trimmed());  // location
        query.addBindValue(fields[4].trimmed());  // playing_surface
        query.addBindValue(fields[5].trimmed());  // league
        query.addBindValue(fields[6].trimmed());  // date_opened
        query.addBindValue(distance);  // distance_to_center (cleaned up)
        query.addBindValue(fields[8].trimmed());  // ballpark_typology
        query.addBindValue(fields[9].trimmed());  // roof_type

        if (!query.exec()) {
            qDebug() << "Error inserting record:" << query.lastError().text();
            qDebug() << "Failed data:" << fields;
            qDebug() << "SQL error:" << query.lastError().text();
        }
    }

    file.close();
    
    // Verify the import
    QSqlQuery countQuery;
    countQuery.exec("SELECT COUNT(*) FROM Teams");
    if (countQuery.next()) {
        int count = countQuery.value(0).toInt();
        qDebug() << "Successfully imported" << count << "teams";
    }
    
    return true;
} 