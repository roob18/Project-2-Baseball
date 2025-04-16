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

void Database::loadStadiumMap()
{
    QSqlQuery query("SELECT * FROM Teams");
    while (query.next()) {
        StadiumInfo info;
        info.teamName = query.value("team_name").toString();
        info.stadiumName = query.value("stadium_name").toString();
        info.seatingCapacity = query.value("seating_capacity").toInt();
        info.location = query.value("location").toString();
        info.playingSurface = query.value("playing_surface").toString();
        info.league = query.value("league").toString();
        info.dateOpened = query.value("date_opened").toString();
        info.distanceToCenter = query.value("distance_to_center").toInt();
        info.ballparkTypology = query.value("ballpark_typology").toString();
        info.roofType = query.value("roof_type").toString();
        
        // Load souvenirs for this team
        QSqlQuery souvenirQuery;
        souvenirQuery.prepare("SELECT item_name, price FROM Souvenirs WHERE team_id = ?");
        souvenirQuery.addBindValue(query.value("id").toInt());
        souvenirQuery.exec();
        
        while (souvenirQuery.next()) {
            info.souvenirs.append(qMakePair(
                souvenirQuery.value("item_name").toString(),
                souvenirQuery.value("price").toDouble()
            ));
        }
        
        // Insert into our custom HashMap
        stadiumMap.insert(info.teamName, info);
    }
}

bool Database::initialize()
{
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }

    createTables();
    insertInitialData();
    initializeSouvenirs();
    loadStadiumMap();  // Load data into our custom HashMap
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

    // Create Souvenirs table
    query.exec("DROP TABLE IF EXISTS Souvenirs");
    query.exec(
        "CREATE TABLE Souvenirs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "team_id INTEGER,"
        "item_name TEXT NOT NULL,"
        "price REAL NOT NULL,"
        "FOREIGN KEY(team_id) REFERENCES Teams(id)"
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

void Database::initializeSouvenirs()
{
    // First, clear existing souvenirs
    QSqlQuery clearQuery;
    clearQuery.exec("DELETE FROM Souvenirs");
    
    // Get all teams
    QSqlQuery query;
    query.exec("SELECT id, team_name FROM Teams");
    
    // Standard souvenir items and prices
    struct SouvenirItem {
        QString name;
        double price;
    };
    
    QVector<SouvenirItem> defaultItems = {
        {"Baseball cap", 19.99},
        {"Baseball bat", 89.39},
        {"Team pennant", 17.99},
        {"Autographed baseball", 29.99},
        {"Team jersey", 199.99}
    };

    // Add default souvenirs for each team
    while (query.next()) {
        int teamId = query.value(0).toInt();
        QString teamName = query.value(1).toString();
        qDebug() << "Adding souvenirs for team:" << teamName;
        
        for (const auto &item : defaultItems) {
            QSqlQuery insertQuery;
            insertQuery.prepare("INSERT INTO Souvenirs (team_id, item_name, price) VALUES (?, ?, ?)");
            insertQuery.addBindValue(teamId);
            insertQuery.addBindValue(item.name);
            insertQuery.addBindValue(item.price);
            
            if (!insertQuery.exec()) {
                qDebug() << "Failed to add souvenir" << item.name << "for team" << teamName 
                        << ":" << insertQuery.lastError().text();
            }
        }
    }
    
    // Verify the insertion
    QSqlQuery countQuery;
    countQuery.exec("SELECT COUNT(*) FROM Souvenirs");
    if (countQuery.next()) {
        qDebug() << "Total souvenirs added:" << countQuery.value(0).toInt();
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

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    // Skip header row
    QString header = in.readLine();
    int teamsAdded = 0;

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

        // Check if team already exists
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM Teams WHERE team_name = ? COLLATE NOCASE");
        checkQuery.addBindValue(fields[0].trimmed());
        checkQuery.exec();
        
        if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
            qDebug() << "Skipping existing team:" << fields[0].trimmed();
            continue;  // Skip this team as it already exists
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
        } else {
            teamsAdded++;
            
            // Add default souvenirs for the new team
            int teamId = query.lastInsertId().toInt();
            QVector<QPair<QString, double>> defaultItems = {
                {"Baseball cap", 19.99},
                {"Baseball bat", 89.39},
                {"Team pennant", 17.99},
                {"Autographed baseball", 29.99},
                {"Team jersey", 199.99}
            };
            
            for (const auto &item : defaultItems) {
                QSqlQuery souvenirQuery;
                souvenirQuery.prepare("INSERT INTO Souvenirs (team_id, item_name, price) VALUES (?, ?, ?)");
                souvenirQuery.addBindValue(teamId);
                souvenirQuery.addBindValue(item.first);
                souvenirQuery.addBindValue(item.second);
                souvenirQuery.exec();
            }
        }
    }

    file.close();
    
    // Report the results
    if (teamsAdded > 0) {
        qDebug() << "Successfully added" << teamsAdded << "new teams";
        return true;
    } else {
        qDebug() << "No new teams were added (all teams already exist)";
        return false;
    }
}

QSqlQuery Database::getSouvenirs(const QString &teamName)
{
    QSqlQuery query;
    query.prepare(
        "SELECT s.id, s.item_name, s.price "
        "FROM Souvenirs s "
        "JOIN Teams t ON s.team_id = t.id "
        "WHERE t.team_name = ? "
        "ORDER BY s.item_name"
    );
    query.addBindValue(teamName);
    query.exec();
    return query;
}

bool Database::addSouvenir(const QString &teamName, const QString &itemName, double price)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Souvenirs (team_id, item_name, price) "
        "SELECT id, ?, ? FROM Teams WHERE team_name = ?"
    );
    query.addBindValue(itemName);
    query.addBindValue(price);
    query.addBindValue(teamName);
    return query.exec();
}

bool Database::updateSouvenirPrice(int souvenirId, double newPrice)
{
    QSqlQuery query;
    query.prepare("UPDATE Souvenirs SET price = ? WHERE id = ?");
    query.addBindValue(newPrice);
    query.addBindValue(souvenirId);
    return query.exec();
}

bool Database::deleteSouvenir(int souvenirId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Souvenirs WHERE id = ?");
    query.addBindValue(souvenirId);
    return query.exec();
}

StadiumInfo Database::getStadiumInfo(const QString &teamName) const
{
    StadiumInfo info;
    if (!stadiumMap.get(teamName, info)) {
        qDebug() << "Team not found:" << teamName;
    }
    return info;
}

QVector<StadiumInfo> Database::getAllStadiums() const
{
    QVector<StadiumInfo> stadiums;
    auto entries = stadiumMap.getAllEntries();
    for (const auto &entry : entries) {
        stadiums.append(entry.second);
    }
    return stadiums;
}

bool Database::updateSouvenirInMap(const QString &teamName, const QString &itemName, double newPrice)
{
    StadiumInfo info;
    if (!stadiumMap.get(teamName, info)) {
        return false;
    }
    
    for (auto &souvenir : info.souvenirs) {
        if (souvenir.first == itemName) {
            souvenir.second = newPrice;
            stadiumMap.insert(teamName, info);  // Update the map
            return true;
        }
    }
    return false;
} 