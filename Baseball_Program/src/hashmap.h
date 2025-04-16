#ifndef HASHMAP_H
#define HASHMAP_H

#include <QString>
#include <QVector>
#include <functional>

// Stadium information structure
struct StadiumInfo {
    QString teamName;
    QString stadiumName;
    int seatingCapacity;
    QString location;
    QString playingSurface;
    QString league;
    QString dateOpened;
    int distanceToCenter;
    QString ballparkTypology;
    QString roofType;
    QVector<QPair<QString, double>> souvenirs;  // List of souvenirs and their prices
};

// Node structure for hash table
template<typename K, typename V>
struct HashNode {
    K key;
    V value;
    HashNode* next;
    
    HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

// Custom HashMap implementation
template<typename K, typename V>
class HashMap {
private:
    static const int TABLE_SIZE = 50;  // Fixed size for simplicity
    HashNode<K, V>* table[TABLE_SIZE];
    
    // Hash function for QString keys
    int hash(const QString& key) const {
        int hash = 0;
        for(QChar c : key) {
            hash = (hash * 31 + c.unicode()) % TABLE_SIZE;
        }
        return hash;
    }
    
    // Hash function for int keys
    int hash(const int& key) const {
        return key % TABLE_SIZE;
    }

public:
    HashMap() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        clear();
    }
    
    void insert(const K& key, const V& value) {
        int index = hash(key);
        HashNode<K, V>* node = table[index];
        
        // Check if key already exists
        while(node != nullptr) {
            if(node->key == key) {
                node->value = value;  // Update value if key exists
                return;
            }
            node = node->next;
        }
        
        // Create new node
        HashNode<K, V>* newNode = new HashNode<K, V>(key, value);
        newNode->next = table[index];
        table[index] = newNode;
    }
    
    bool get(const K& key, V& value) const {
        int index = hash(key);
        HashNode<K, V>* node = table[index];
        
        while(node != nullptr) {
            if(node->key == key) {
                value = node->value;
                return true;
            }
            node = node->next;
        }
        return false;
    }
    
    bool remove(const K& key) {
        int index = hash(key);
        HashNode<K, V>* node = table[index];
        HashNode<K, V>* prev = nullptr;
        
        while(node != nullptr) {
            if(node->key == key) {
                if(prev == nullptr) {
                    table[index] = node->next;
                } else {
                    prev->next = node->next;
                }
                delete node;
                return true;
            }
            prev = node;
            node = node->next;
        }
        return false;
    }
    
    void clear() {
        for(int i = 0; i < TABLE_SIZE; i++) {
            HashNode<K, V>* node = table[i];
            while(node != nullptr) {
                HashNode<K, V>* temp = node;
                node = node->next;
                delete temp;
            }
            table[i] = nullptr;
        }
    }
    
    // Iterator for traversing all entries
    QVector<QPair<K, V>> getAllEntries() const {
        QVector<QPair<K, V>> entries;
        for(int i = 0; i < TABLE_SIZE; i++) {
            HashNode<K, V>* node = table[i];
            while(node != nullptr) {
                entries.push_back(qMakePair(node->key, node->value));
                node = node->next;
            }
        }
        return entries;
    }
};

#endif // HASHMAP_H 