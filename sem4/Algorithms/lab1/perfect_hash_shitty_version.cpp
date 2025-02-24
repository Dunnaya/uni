#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <limits>
#include <cmath>
#include <iomanip>
#include <unordered_set>

using namespace std;

struct HashFunction {
    uint64_t a, b;
    uint64_t p, m;
    
    HashFunction(int _m) : m(_m) {
        random_device rd;
        mt19937_64 gen(rd());
        uniform_int_distribution<uint64_t> dist(1, UINT64_MAX / 2);
        a = dist(gen);
        b = dist(gen);
        p = 1000000007;
    }
    
    HashFunction(uint64_t _a, uint64_t _b, uint64_t _p, uint64_t _m) 
        : a(_a), b(_b), p(_p), m(_m) {}
    
    static uint64_t doubleToInt(double key) {
        int64_t scaled = static_cast<int64_t>(round(key * 1000000.0));
        uint64_t folded = (scaled < 0) ? 
            static_cast<uint64_t>(-scaled) * 2 + 1 : 
            static_cast<uint64_t>(scaled) * 2;
        return folded;
    }
    
    int operator()(double key) const {
        if (isnan(key) || isinf(key)) {
            throw invalid_argument("Invalid key: NaN or Inf not supported");
        }
        uint64_t k = doubleToInt(key);
        return static_cast<int>(((a * k + b) % p) % m);
    }
};

struct HashTableLevel {
    vector<double> data;
    HashFunction hashFunc;
    bool useSecondHash;
    
    // Constructor for single-element or empty buckets
    HashTableLevel(double value = numeric_limits<double>::quiet_NaN()) 
        : data(1, value), hashFunc(1), useSecondHash(false) {}
    
    // Constructor for multiple elements
    HashTableLevel(int size, const HashFunction& func) 
        : data(size, numeric_limits<double>::quiet_NaN()), 
          hashFunc(func), 
          useSecondHash(true) {}
};

class PerfectHashTable {
private:
    vector<vector<double> > buckets;
    vector<HashTableLevel> secondLevelTables;
    HashFunction firstLevelHash;
    size_t size;

    static bool doubleEquals(double a, double b) {
        if (a == b) return true;
        double epsilon = 1e-6;
        double abs_diff = fabs(a - b);
        double max_abs = max(fabs(a), fabs(b));
        return max_abs < epsilon ? abs_diff < epsilon : abs_diff / max_abs < epsilon;
    }

public:
    PerfectHashTable(const vector<double>& keys) : firstLevelHash(keys.size()), size(keys.size()) {
        if (keys.empty()) {
            throw invalid_argument("Empty key set not allowed");
        }

        buckets.resize(size);
        cout << "\nFirst level hashing:\n";
        for (double key : keys) {
            int index = firstLevelHash(key);
            buckets[index].push_back(key);
            cout << fixed << setprecision(6) 
                 << "Key " << key << " -> bucket " << index 
                 << " (hash value: " << HashFunction::doubleToInt(key) << ")" << endl;
        }
        
        secondLevelTables.reserve(size);
        cout << "\nSecond level hashing:\n";
        
        for (size_t i = 0; i < size; ++i) {
            if (buckets[i].empty()) {
                secondLevelTables.emplace_back();  // Empty bucket
                continue;
            }
            
            if (buckets[i].size() == 1) {
                // Store single element directly without second hash
                secondLevelTables.emplace_back(buckets[i][0]);
                cout << "Bucket " << i << ": Single key " << fixed << setprecision(6) 
                     << buckets[i][0] << " stored directly\n";
                continue;
            }
            
            // Multiple elements - need second level hash
            int bucketSize = buckets[i].size();
            int tableSize = bucketSize * bucketSize;
            
            bool success = false;
            int attempts = 0;
            const int maxAttempts = 1000;
            HashFunction bestHash(tableSize);
            vector<double> bestTable;
            
            while (!success && attempts++ < maxAttempts) {
                HashFunction h(tableSize);
                vector<double> tempTable(tableSize, numeric_limits<double>::quiet_NaN());
                unordered_set<int> usedPositions;
                success = true;
                
                for (double key : buckets[i]) {
                    int index = h(key);
                    if (usedPositions.count(index) > 0) {
                        success = false;
                        break;
                    }
                    usedPositions.insert(index);
                    tempTable[index] = key;
                }
                
                if (success) {
                    bestHash = h;
                    bestTable = tempTable;
                }
            }
            
            if (!success) {
                throw runtime_error("Failed to find perfect hash function for bucket " + to_string(i));
            }
            
            // Store the successful hash function and table
            secondLevelTables.emplace_back(tableSize, bestHash);
            secondLevelTables.back().data = bestTable;
            
            // Print successful insertions
            cout << "Bucket " << i << " (second level hashing):\n";
            for (double key : buckets[i]) {
                int index = bestHash(key);
                cout << "  Key " << fixed << setprecision(6) 
                     << key << " -> position " << index 
                     << " (hash value: " << HashFunction::doubleToInt(key) << ")" << endl;
            }
        }
    }

    bool lookup(double key) const {
        try {
            int firstIndex = firstLevelHash(key);
            
            if (firstIndex >= buckets.size()) {
                cout << "Debug: key=" << fixed << setprecision(6) << key 
                     << " firstIndex=" << firstIndex << " (out of bounds)" << endl;
                return false;
            }
            
            const HashTableLevel& level = secondLevelTables[firstIndex];
            
            if (!level.useSecondHash) {
                // For empty or single-element buckets
                bool found = !isnan(level.data[0]) && doubleEquals(level.data[0], key);
                cout << "Debug: key=" << fixed << setprecision(6) << key 
                     << " firstIndex=" << firstIndex 
                     << " (direct lookup, stored=" << level.data[0] << ")" << endl;
                return found;
            }
            
            // Second level lookup for multiple-element buckets
            int secondIndex = level.hashFunc(key);
            cout << "Debug: key=" << fixed << setprecision(6) << key 
                 << " firstIndex=" << firstIndex 
                 << " secondIndex=" << secondIndex 
                 << " (hash value: " << HashFunction::doubleToInt(key) << ")";
            
            if (secondIndex >= static_cast<int>(level.data.size())) {
                cout << " (index out of bounds)" << endl;
                return false;
            }
            
            double storedValue = level.data[secondIndex];
            cout << " stored=" << storedValue << endl;
            
            return !isnan(storedValue) && doubleEquals(storedValue, key);
            
        } catch (const exception& e) {
            cerr << "Error during lookup: " << e.what() << endl;
            return false;
        }
    }
    
    void printStats() const {
        cout << "\nPerfect Hash Table Statistics:\n";
        cout << "Total number of keys: " << size << "\n";
        cout << "Number of first-level buckets: " << buckets.size() << "\n";
        
        int directStorageCount = 0;
        int secondLevelCount = 0;
        int totalSecondLevelSize = 0;
        int maxBucketSize = 0;
        
        for (size_t i = 0; i < buckets.size(); ++i) {
            if (!buckets[i].empty()) {
                maxBucketSize = max(maxBucketSize, static_cast<int>(buckets[i].size()));
                
                if (buckets[i].size() == 1) {
                    directStorageCount++;
                    cout << "\nBucket " << i << ": Single key stored directly\n";
                    cout << "Key: " << fixed << setprecision(6) << buckets[i][0] 
                         << " (hash value: " << HashFunction::doubleToInt(buckets[i][0]) << ")\n";
                }
                else if (buckets[i].size() > 1) {
                    secondLevelCount++;
                    cout << "\nBucket " << i << ": " << buckets[i].size() 
                         << " keys, table size: " << secondLevelTables[i].data.size() << "\n";
                    cout << "Keys in bucket " << i << ":\n";
                    for (double key : buckets[i]) {
                        cout << fixed << setprecision(6) << key 
                             << " (hash value: " << HashFunction::doubleToInt(key) << ")\n";
                    }
                    
                    cout << "Second level table " << i << " contents:\n";
                    for (size_t j = 0; j < secondLevelTables[i].data.size(); j++) {
                        if (!isnan(secondLevelTables[i].data[j])) {
                            cout << "Position " << j << ": " << secondLevelTables[i].data[j] 
                                 << " (hash value: " << HashFunction::doubleToInt(secondLevelTables[i].data[j]) << ")\n";
                        }
                    }
                    totalSecondLevelSize += secondLevelTables[i].data.size();
                }
            }
        }
        
        cout << "\nSummary:\n";
        cout << "Max bucket size: " << maxBucketSize << "\n";
        cout << "Buckets with direct storage: " << directStorageCount << "\n";
        cout << "Buckets with second-level hashing: " << secondLevelCount << "\n";
        cout << "Total second-level storage: " << totalSecondLevelSize << " slots\n";
    }
};

int main() {
    try {
        vector<double> keys;
        //{-10.5, 22.3333, 0.0078, 40.1, -52.6666, 60.4444, 0.0001, 85.9999, 90.0};
        keys.push_back(-10.5);
        keys.push_back(22.3333);
        keys.push_back(0.0078);
        keys.push_back(40.1);
        keys.push_back(-52.6666);
        keys.push_back(60.4444);
        keys.push_back(0.0001);
        keys.push_back(85.9999);
        keys.push_back(90.0);

        cout << "Creating perfect hash table with keys:";
        for (double key : keys) {
            cout << fixed << setprecision(6) << " " << key;
        }
        cout << endl;

        PerfectHashTable hashTable(keys);
        hashTable.printStats();
        
        vector<double> testKeys;
        //{22.3333, 85.9999, 99.9, 0.0078, -10.5, 0.0002};
        testKeys.push_back(22.3333);
        testKeys.push_back(85.9999);
        testKeys.push_back(99.9);
        testKeys.push_back(0.0078);
        testKeys.push_back(-10.5);
        testKeys.push_back(0.0002);
        
        cout << "\nTesting lookups:\n";
        for (double key : testKeys) {
            bool found = hashTable.lookup(key);
            cout << fixed << setprecision(6) 
                 << "Key " << key << " -> " << (found ? "Found" : "Not found") << endl;
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}