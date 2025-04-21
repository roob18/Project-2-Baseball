#include "database.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStringConverter>
#include <QSqlError>
#include <QStringList>
#include <QDebug>

Database::Database(QObject *parent)
    : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // Use in-memory database for this application
}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

void Database::loadStadiumMap()
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM teams");
    
    while (query.next()) {
        StadiumInfo info;
        info.teamName = query.value("team_name").toString();
        info.stadiumName = query.value("stadium_name").toString();
        info.seatingCapacity = query.value("capacity").toInt();
        info.location = query.value("location").toString();
        info.playingSurface = query.value("surface").toString();
        info.league = query.value("league").toString();
        info.dateOpened = query.value("date_opened").toString();
        info.distanceToCenter = query.value("center_field").toInt();
        info.ballparkTypology = query.value("typology").toString();
        info.roofType = query.value("roof").toString();
        
        // Load souvenirs for this team
        QSqlQuery souvenirQuery(db);
        souvenirQuery.prepare("SELECT item_name, price FROM souvenirs WHERE team_name = :team");
        souvenirQuery.bindValue(":team", info.teamName);
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

    if (!createTables()) {
        return false;
    }

    // Start a transaction for faster inserts
    db.transaction();
    
    insertInitialData();  // This now handles both teams and souvenirs
    loadStadiumMap();     // Update the stadium map with the new data
    
    // Commit the transaction
    if (!db.commit()) {
        qDebug() << "Error committing data:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

bool Database::createTables()
{
    QSqlQuery query(db);
    
    // Create teams table
    if (!query.exec("CREATE TABLE IF NOT EXISTS teams ("
                   "team_name TEXT PRIMARY KEY,"
                   "stadium_name TEXT,"
                   "capacity INTEGER,"
                   "location TEXT,"
                   "surface TEXT,"
                   "league TEXT,"
                   "date_opened TEXT,"
                   "center_field INTEGER,"
                   "typology TEXT,"
                   "roof TEXT)")) {
        qDebug() << "Error creating teams table:" << query.lastError().text();
        return false;
    }

    // Create souvenirs table
    if (!query.exec("CREATE TABLE IF NOT EXISTS souvenirs ("
                   "team_name TEXT,"
                   "item_name TEXT,"
                   "price REAL,"
                   "PRIMARY KEY (team_name, item_name),"
                   "FOREIGN KEY (team_name) REFERENCES teams(team_name))")) {
        qDebug() << "Error creating souvenirs table:" << query.lastError().text();
        return false;
    }

    return true;
}

void Database::insertInitialData()
{
    // Insert initial teams
    insertTeam("Boston Red Sox", "Fenway Park", 37755, "Boston, MA", "Grass", 
               "American", "1912-04-20", 390, "Jewel Box", "Open");
               
    insertTeam("New York Yankees", "Yankee Stadium", 47309, "Bronx, NY", "Grass",
               "American", "2009-04-16", 408, "Retro-Modern", "Open");
               
    insertTeam("Los Angeles Dodgers", "Dodger Stadium", 56000, "Los Angeles, CA", "Grass",
               "National", "1962-04-10", 395, "Modern", "Open");

    // Add default souvenirs for each team
    QStringList teams = {"Boston Red Sox", "New York Yankees", "Los Angeles Dodgers"};
    QVector<QPair<QString, double>> defaultSouvenirs = {
        {"Baseball Cap", 25.99},
        {"Team Jersey", 199.99},
        {"Baseball", 15.99},
        {"Team Pennant", 12.99},
        {"Team Baseball Bat", 89.99}
    };

    for (const QString &team : teams) {
        for (const auto &souvenir : defaultSouvenirs) {
            QSqlQuery query(db);
            query.prepare("INSERT INTO souvenirs (team_name, item_name, price) VALUES (:team, :item, :price)");
            query.bindValue(":team", team);
            query.bindValue(":item", souvenir.first);
            query.bindValue(":price", souvenir.second);
            if (!query.exec()) {
                qDebug() << "Error adding souvenir" << souvenir.first << "for team" << team 
                        << ":" << query.lastError().text();
            }
        }
    }
}

void Database::initializeSouvenirs()
{
    // This function is now handled by insertInitialData
    // Keep it empty as it's called from initialize()
}

QSqlQuery Database::getTeamInfo(const QString &teamName)
{
    QSqlQuery query(db);
    query.prepare(
        "SELECT team_name, stadium_name, "
        "printf('%,d', capacity) as capacity, "
        "location, surface, league, date_opened, "
        "CASE "
        "  WHEN center_field > 0 THEN printf('%d feet (%d m)', center_field, CAST(ROUND(center_field * 0.3048) AS INTEGER)) "
        "  ELSE '0' "
        "END as center_field, "
        "typology, roof "
        "FROM teams WHERE TRIM(team_name) = TRIM(:team)");
    query.bindValue(":team", teamName);
    if (!query.exec()) {
        qDebug() << "Error getting team info:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getAllTeamsSortedByTeamName()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(team_name) as team_name, TRIM(stadium_name) as stadium_name "
                   "FROM teams WHERE team_name IS NOT NULL AND team_name != '' "
                   "ORDER BY TRIM(team_name)")) {
        qDebug() << "Error getting teams by name:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getAllTeamsSortedByStadiumName()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(team_name) as team_name, TRIM(stadium_name) as stadium_name "
                   "FROM teams WHERE stadium_name IS NOT NULL AND stadium_name != '' "
                   "ORDER BY TRIM(stadium_name)")) {
        qDebug() << "Error getting teams by stadium:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getAmericanLeagueTeams()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(team_name) as team_name, TRIM(stadium_name) as stadium_name "
                   "FROM teams WHERE TRIM(UPPER(league)) = 'AMERICAN' "
                   "ORDER BY TRIM(team_name)")) {
        qDebug() << "Error getting American League teams:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getNationalLeagueTeams()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(team_name) as team_name, TRIM(stadium_name) as stadium_name "
                   "FROM teams WHERE TRIM(UPPER(league)) = 'NATIONAL' "
                   "ORDER BY TRIM(team_name)")) {
        qDebug() << "Error getting National League teams:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getTeamsByTypology()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(stadium_name) as stadium_name, TRIM(team_name) as team_name, "
                   "TRIM(typology) as typology FROM teams "
                   "WHERE typology IS NOT NULL AND typology != '' "
                   "ORDER BY TRIM(typology), TRIM(team_name)")) {
        qDebug() << "Error getting teams by typology:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getOpenRoofTeams()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(team_name) as team_name FROM teams "
                   "WHERE TRIM(UPPER(roof)) = 'OPEN' AND team_name IS NOT NULL "
                   "ORDER BY TRIM(team_name)")) {
        qDebug() << "Error getting open roof teams:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getTeamsByDateOpened()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT TRIM(stadium_name) as stadium_name, TRIM(team_name) as team_name, "
                   "TRIM(date_opened) as date_opened FROM teams "
                   "WHERE date_opened IS NOT NULL AND date_opened != '' "
                   "ORDER BY date_opened")) {
        qDebug() << "Error getting teams by date:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getTeamsByCapacity()
{
    QSqlQuery query(db);
    if (!query.exec("SELECT stadium_name, team_name, capacity FROM teams ORDER BY capacity DESC")) {
        qDebug() << "Error getting teams by capacity:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getTeamsWithGreatestCenterField()
{
    QSqlQuery query(db);
    if (!query.exec(
        "WITH MaxDistance AS ("
        "   SELECT MAX(center_field) as max_distance "
        "   FROM teams "
        "   WHERE center_field > 0"
        ") "
        "SELECT TRIM(stadium_name) as stadium_name, "
        "       TRIM(team_name) as team_name, "
        "       printf('%d feet (%d m)', center_field, CAST(ROUND(center_field * 0.3048) AS INTEGER)) as center_field "
        "FROM teams "
        "WHERE center_field = (SELECT max_distance FROM MaxDistance) "
        "ORDER BY team_name")) {
        qDebug() << "Error getting greatest center field:" << query.lastError().text();
    }
    return query;
}

QSqlQuery Database::getTeamsWithSmallestCenterField()
{
    QSqlQuery query(db);
    if (!query.exec(
        "WITH MinDistance AS ("
        "   SELECT MIN(center_field) as min_distance "
        "   FROM teams "
        "   WHERE center_field > 0"
        ") "
        "SELECT TRIM(stadium_name) as stadium_name, "
        "       TRIM(team_name) as team_name, "
        "       printf('%d feet (%d m)', center_field, CAST(ROUND(center_field * 0.3048) AS INTEGER)) as center_field "
        "FROM teams "
        "WHERE center_field = (SELECT min_distance FROM MinDistance) "
        "ORDER BY team_name")) {
        qDebug() << "Error getting smallest center field:" << query.lastError().text();
    }
    return query;
}

bool Database::importFromCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    // Skip header line
    if (!in.atEnd()) {
        in.readLine();
    }

    db.transaction();

    // Clear existing data first
    QSqlQuery clearQuery(db);
    if (!clearQuery.exec("DELETE FROM souvenirs")) {
        qDebug() << "Error clearing souvenirs:" << clearQuery.lastError().text();
        db.rollback();
        return false;
    }
    if (!clearQuery.exec("DELETE FROM teams")) {
        qDebug() << "Error clearing teams:" << clearQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Default souvenirs that each team should have
    QVector<QPair<QString, double>> defaultSouvenirs = {
        {"Baseball Cap", 25.99},
        {"Team Jersey", 199.99},
        {"Baseball", 15.99},
        {"Team Pennant", 12.99},
        {"Team Baseball Bat", 89.99}
    };

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields;
        bool inQuotes = false;
        QString currentField;
        
        // Custom CSV parsing to handle quoted fields
        for (int i = 0; i < line.length(); ++i) {
            QChar currentChar = line[i];
            
            if (currentChar == '"') {
                inQuotes = !inQuotes;
            } else if (currentChar == ',' && !inQuotes) {
                fields.append(currentField.trimmed());
                currentField.clear();
            } else {
                currentField += currentChar;
            }
        }
        // Add the last field
        fields.append(currentField.trimmed());
        
        // Remove quotes from fields
        for (int i = 0; i < fields.size(); ++i) {
            fields[i].remove('"');
        }
        
        if (fields.size() >= 10) {
            QString teamName = fields[0].trimmed();
            
            // Skip empty team names
            if (teamName.isEmpty()) {
                continue;
            }

            QString stadiumName = fields[1].trimmed();
            
            // Clean up capacity data
            QString capacityStr = fields[2].trimmed();
            capacityStr.remove(QRegularExpression("[^0-9]"));
            int capacity = capacityStr.toInt();
            
            QString location = fields[3].trimmed();
            QString surface = fields[4].trimmed();
            QString league = fields[5].trimmed();
            
            // Clean up date opened
            QString dateOpened = fields[6].trimmed();
            QRegularExpression yearRegex("\\b\\d{4}\\b");
            auto match = yearRegex.match(dateOpened);
            if (match.hasMatch()) {
                dateOpened = match.captured(0);
            }
            
            // Clean up center field distance
            QString centerFieldStr = fields[7].trimmed();
            // Extract just the feet value (before any parentheses)
            QRegularExpression feetRegex("(\\d+)\\s*(?:feet|$)");
            auto feetMatch = feetRegex.match(centerFieldStr);
            int centerField = 0;
            if (feetMatch.hasMatch()) {
                centerField = feetMatch.captured(1).toInt();
            } else {
                // If no "feet" found, try to extract just the first number
                QRegularExpression numberRegex("\\d+");
                auto numberMatch = numberRegex.match(centerFieldStr);
                if (numberMatch.hasMatch()) {
                    centerField = numberMatch.captured(0).toInt();
                }
            }
            
            // Validate the center field value
            if (centerField <= 0 || centerField > 1000) { // Sanity check for reasonable values
                centerField = 0;
            }
            
            QString typology = fields[8].trimmed();
            QString roof = fields[9].trimmed();

            // Validate and clean data
            if (league.isEmpty()) league = "Unknown";
            if (surface.isEmpty()) surface = "Unknown";
            if (typology.isEmpty()) typology = "Unknown";
            if (roof.isEmpty()) roof = "Unknown";
            
            // Ensure capacity and center field are valid
            if (capacity <= 0) capacity = 0;
            if (centerField <= 0) centerField = 0;

            qDebug() << "Inserting team:" << teamName;
            if (!insertTeam(teamName, stadiumName, capacity, location, surface,
                          league, dateOpened, centerField, typology, roof)) {
                qDebug() << "Error inserting team:" << teamName;
                db.rollback();
                file.close();
                return false;
            }

            // Add default souvenirs for this team
            for (const auto &souvenir : defaultSouvenirs) {
                QSqlQuery souvenirQuery(db);
                souvenirQuery.prepare(
                    "INSERT INTO souvenirs (team_name, item_name, price) "
                    "VALUES (:team, :item, :price)"
                );
                souvenirQuery.bindValue(":team", teamName);
                souvenirQuery.bindValue(":item", souvenir.first);
                souvenirQuery.bindValue(":price", souvenir.second);
                
                if (!souvenirQuery.exec()) {
                    qDebug() << "Error adding souvenir" << souvenir.first 
                            << "for team" << teamName 
                            << ":" << souvenirQuery.lastError().text();
                    db.rollback();
                    file.close();
                    return false;
                }
                qDebug() << "Added souvenir" << souvenir.first << "for team" << teamName;
            }
        }
    }

    if (!db.commit()) {
        qDebug() << "Error committing data:" << db.lastError().text();
        db.rollback();
        file.close();
        return false;
    }

    file.close();
    
    // Reload the stadium map after import
    loadStadiumMap();
    
    return true;
}

bool Database::insertTeam(const QString &teamName, const QString &stadiumName,
                         int capacity, const QString &location, const QString &surface,
                         const QString &league, const QString &dateOpened,
                         int centerField, const QString &typology, const QString &roof)
{
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO teams (team_name, stadium_name, capacity, location, "
                 "surface, league, date_opened, center_field, typology, roof) "
                 "VALUES (:team, :stadium, :capacity, :location, :surface, :league, "
                 ":opened, :center, :typology, :roof)");
    
    query.bindValue(":team", teamName);
    query.bindValue(":stadium", stadiumName);
    query.bindValue(":capacity", capacity);
    query.bindValue(":location", location);
    query.bindValue(":surface", surface);
    query.bindValue(":league", league);
    query.bindValue(":opened", dateOpened);
    query.bindValue(":center", centerField);
    query.bindValue(":typology", typology);
    query.bindValue(":roof", roof);

    if (!query.exec()) {
        qDebug() << "Error inserting team:" << query.lastError().text();
        return false;
    }

    return true;
}

QVector<QPair<QString, double>> Database::getSouvenirs(const QString &teamName)
{
    QVector<QPair<QString, double>> souvenirs;
    
    QSqlQuery query(db);
    query.prepare(
        "SELECT item_name, price FROM souvenirs "
        "WHERE team_name = :team_name "
        "ORDER BY item_name"
    );
    query.bindValue(":team_name", teamName);
    
    qDebug() << "Fetching souvenirs for team:" << teamName;
    
    if (query.exec()) {
        while (query.next()) {
            QString itemName = query.value("item_name").toString();
            double price = query.value("price").toDouble();
            souvenirs.append(qMakePair(itemName, price));
            qDebug() << "Found souvenir:" << itemName << "Price:" << price;
        }
    } else {
        qDebug() << "Error fetching souvenirs:" << query.lastError().text();
    }
    
    qDebug() << "Total souvenirs found:" << souvenirs.size();
    return souvenirs;
}

bool Database::addSouvenir(const QString &teamName, const QString &itemName, double price)
{
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO souvenirs (team_name, item_name, price) "
        "VALUES (:team_name, :item_name, :price)"
    );
    query.bindValue(":team_name", teamName);
    query.bindValue(":item_name", itemName);
    query.bindValue(":price", price);
    
    if (!query.exec()) {
        qDebug() << "Error adding souvenir:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::updateSouvenirPrice(const QString &teamName, const QString &itemName, double newPrice)
{
    QSqlQuery query(db);
    query.prepare(
        "UPDATE souvenirs "
        "SET price = :price "
        "WHERE TRIM(team_name) = TRIM(:team_name) "
        "AND TRIM(item_name) = TRIM(:item_name)"
    );
    query.bindValue(":team_name", teamName);
    query.bindValue(":item_name", itemName);
    query.bindValue(":price", newPrice);
    
    if (!query.exec()) {
        qDebug() << "Error updating souvenir price:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Database::deleteSouvenir(const QString &teamName, const QString &itemName)
{
    QSqlQuery query(db);
    query.prepare(
        "DELETE FROM souvenirs "
        "WHERE TRIM(team_name) = TRIM(:team_name) "
        "AND TRIM(item_name) = TRIM(:item_name)"
    );
    query.bindValue(":team_name", teamName);
    query.bindValue(":item_name", itemName);
    
    if (!query.exec()) {
        qDebug() << "Error deleting souvenir:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
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