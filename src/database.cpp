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
    // Use in-memory database so imported CSV data is only kept for the current run.
    // When the program is closed or rebuilt, the database resets to the three preloaded teams.
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("baseball.db");
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

    // Create distances table
    if (!query.exec("CREATE TABLE IF NOT EXISTS distances ("
                   "from_team TEXT,"
                   "to_team TEXT,"
                   "distance INTEGER,"
                   "PRIMARY KEY (from_team, to_team))")) {
        qDebug() << "Error creating distances table:" << query.lastError().text();
        return false;
    }

    return true;
}

void Database::insertInitialData()
{
    // Insert initial teams
    insertTeam("Boston Red Sox", "Fenway Park", 37731, "Boston, MA", "Grass", 
               "American", "1912", 420, "Jewel Box", "Open");
               
    insertTeam("New York Yankees", "Yankee Stadium", 47422, "Bronx, NY", "Grass",
               "American", "2009", 408, "Retro-Modern", "Open");
               
    insertTeam("Los Angeles Dodgers", "Dodger Stadium", 56000, "Los Angeles, CA", "Grass",
               "National", "1962", 400, "Modern", "Open");

    // Add default souvenirs for each team
    QStringList teams = {"Boston Red Sox", "New York Yankees", "Los Angeles Dodgers"};
    QVector<QPair<QString, double>> defaultSouvenirs = {
        {"Baseball cap", 19.99},
        {"Baseball bat", 89.39},
        {"Team pennant", 17.99},
        {"Autographed baseball", 29.99},
        {"Team jersey", 199.99}
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
    if (!query.exec("SELECT stadium_name, team_name, capacity FROM teams ORDER BY capacity ASC")) {
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

bool Database::importFromCSV(const QStringList &filenames)
{
    if (filenames.isEmpty()) {
        qDebug() << "No files provided for import";
        return false;
    }

    db.transaction();

    bool success = true;
    for (const QString &filename : filenames) {
        if (!importSingleCSV(filename)) {
            qDebug() << "Failed to import file:" << filename;
            success = false;
            break;
        }
    }

    if (success) {
        if (!db.commit()) {
            qDebug() << "Error committing data:" << db.lastError().text();
            db.rollback();
            return false;
        }
        // Reload the stadium map after import
        loadStadiumMap();
    } else {
        db.rollback();
    }

    return success;
}

bool Database::importSingleCSV(const QString &filename)
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

    // Default souvenirs that each team should have
    QVector<QPair<QString, double>> defaultSouvenirs = {
        {"Baseball cap", 19.99},
        {"Baseball bat", 89.39},
        {"Team pennant", 17.99},
        {"Autographed baseball", 29.99},
        {"Team jersey", 199.99}
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

        // Check if team already exists
            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM teams WHERE team_name = :team");
            checkQuery.bindValue(":team", teamName);
        checkQuery.exec();
            checkQuery.next();
            bool teamExists = checkQuery.value(0).toInt() > 0;

            // If team exists, skip it
            if (teamExists) {
                qDebug() << "Team already exists, skipping:" << teamName;
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
                    file.close();
                    return false;
                }
                qDebug() << "Added souvenir" << souvenir.first << "for team" << teamName;
            }
        }
    }

    file.close();
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

bool Database::validateAdmin(const QString &username, const QString &password)
{
    // For now, use a simple hardcoded admin account
    // In a real application, this would check against a secure database
    return (username == "admin" && password == "admin123");
}

bool Database::importDistancesFromCSV(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening distances file:" << file.errorString();
        return false;
    }
    QTextStream in(&file);
    // Skip header line
    if (!in.atEnd()) {
        in.readLine();
    }
    db.transaction();
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() < 3) continue;
        QString from = fields[0].trimmed();
        QString to = fields[1].trimmed();
        bool ok = false;
        double distance = fields[2].trimmed().toDouble(&ok);
        if (!ok || from.isEmpty() || to.isEmpty()) continue;
        QSqlQuery query(db);
        query.prepare("INSERT OR REPLACE INTO distances (from_team, to_team, distance) VALUES (:from, :to, :distance)");
        query.bindValue(":from", from);
        query.bindValue(":to", to);
        query.bindValue(":distance", distance);
        if (!query.exec()) {
            qDebug() << "Error inserting distance:" << query.lastError().text();
            db.rollback();
            file.close();
            return false;
        }
    }
    db.commit();
    file.close();
    return true;
}

QVector<QPair<QString, QPair<QString, double>>> Database::getAllDistances() const
{
    QVector<QPair<QString, QPair<QString, double>>> distances;
    QSqlQuery query(db);
    if (query.exec("SELECT from_team, to_team, distance FROM distances")) {
        while (query.next()) {
            QString from = query.value(0).toString();
            QString to = query.value(1).toString();
            double dist = query.value(2).toDouble();
            distances.append(qMakePair(from, qMakePair(to, dist)));
        }
    }
    return distances;
} 