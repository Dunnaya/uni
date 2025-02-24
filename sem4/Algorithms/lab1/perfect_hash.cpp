#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>
using namespace std;

class PerfectHashTable 
{
    private:
        struct HashFunction 
        {
            uint64_t a, b, m;
            
            HashFunction(int size) : m(size) 
            {
                random_device rd;
                mt19937_64 gen(rd());
                uniform_int_distribution<uint64_t> dist(1, 1000000);
                a = dist(gen);
                b = dist(gen);
            }
            
            int operator()(double key) const 
            {
                // Simple conversion: multiply by 1M and take absolute value
                int64_t k = abs(static_cast<int64_t>(key * 1000000));
                return ((a * k + b) % 1000000007) % m;
            }
        };

        struct Bucket 
        {
            vector<double> data;
            HashFunction* hashFunc;
            bool useSecondHash;
            
            // For empty or single-element buckets
            Bucket() : hashFunc(nullptr), useSecondHash(false) {}
            
            // For buckets needing second-level hash
            Bucket(int size, HashFunction* func) 
                : data(size), hashFunc(func), useSecondHash(true) {}
                
            ~Bucket() 
            {
                delete hashFunc;
            }
        };

        vector<vector<double> > firstLevel;  // Just to store initial distribution
        vector<Bucket> buckets;
        HashFunction firstHash;
        int numKeys;

    public:
        PerfectHashTable(const vector<double>& keys) 
            : firstLevel(keys.size()), buckets(keys.size()), 
            firstHash(keys.size()), numKeys(keys.size()) 
            {
            
            // Check for duplicates first
            unordered_set<double> uniqueKeys;
            bool hasDuplicates = false;
            
            cout << "Checking for duplicate keys:" << endl;
            for (double key : keys) 
            {
                if (uniqueKeys.find(key) != uniqueKeys.end()) 
                {
                    cout << "  Duplicate found: " << key << endl;
                    hasDuplicates = true;
                }
                uniqueKeys.insert(key);
            }
            
            if (hasDuplicates) 
                cout << "Warning: Duplicate keys detected! Using only unique keys." << endl;
            
            // First level distribution (using only unique keys)
            cout << "\nFirst level distribution:\n";
            for (double key : uniqueKeys) 
            {
                int idx = firstHash(key);
                firstLevel[idx].push_back(key);
                cout << key << " -> bucket " << idx << endl;
            }
            
            // Handle each bucket
            cout << "\nSecond level setup:\n";
            for (int i = 0; i < numKeys; i++) 
            {
                auto& bucket = firstLevel[i];
                
                if (bucket.empty()) 
                {
                    continue;  // Leave as default empty bucket
                }
                else if (bucket.size() == 1) 
                {
                    // Store single element directly
                    buckets[i].data = vector<double>(1, bucket[0]);
                    cout << "Bucket " << i << ": storing " << bucket[0] << " directly\n";
                }
                else 
                {
                    // Need second level hashing
                    int size = bucket.size() * bucket.size();
                    buckets[i].hashFunc = new HashFunction(size);
                    buckets[i].data.resize(size);
                    buckets[i].useSecondHash = true;
                    
                    cout << "Bucket " << i << ": creating second level table size " 
                        << size << " for " << bucket.size() << " elements\n";
                    
                    // Try to find a good hash function
                    bool success = false;
                    int maxAttempts = 1000;  // Add a maximum attempt limit to prevent infinite loops
                    int attempts = 0;
                    
                    while (!success && attempts < maxAttempts) 
                    {
                        success = true;
                        attempts++;
                        vector<bool> used(size, false);
                        
                        for (double key : bucket) 
                        {
                            int pos = (*buckets[i].hashFunc)(key);
                            if (used[pos]) 
                            {
                                success = false;
                                delete buckets[i].hashFunc;
                                buckets[i].hashFunc = new HashFunction(size);
                                break;
                            }
                            used[pos] = true;
                            buckets[i].data[pos] = key;
                        }
                    }
                    
                    if (!success) 
                    {
                        cout << "  Warning: Failed to find a suitable hash function after " 
                             << maxAttempts << " attempts.\n";
                    } else {
                        cout << "  Found suitable hash function after " << attempts << " attempts.\n";
                    }
                    
                    // Print positions
                    for (double key : bucket) 
                    {
                        cout << "  " << key << " -> position " 
                            << (*buckets[i].hashFunc)(key) << endl;
                    }
                }
            }
        }
        
        bool lookup(double key) const 
        {
            int firstIdx = firstHash(key);
            const Bucket& bucket = buckets[firstIdx];
            
            if (bucket.data.empty()) 
                return false;
            
            if (!bucket.useSecondHash) 
                return bucket.data[0] == key;  // Direct comparison for single element
            
            // Second level lookup
            int secondIdx = (*bucket.hashFunc)(key);
            return (secondIdx < bucket.data.size()) && (bucket.data[secondIdx] == key);
        }
        
        void printStats() const 
        {
            cout << "\nHash Table Statistics:\n";
            int direct = 0, second = 0, total_slots = 0;
            
            for (int i = 0; i < numKeys; i++) 
            {
                if (!buckets[i].data.empty()) 
                {
                    if (buckets[i].useSecondHash) 
                    {
                        second++;
                        total_slots += buckets[i].data.size();
                    } else {
                        direct++;
                    }
                }
            }
            
            cout << "Buckets with direct storage: " << direct << endl;
            cout << "Buckets with second-level hashing: " << second << endl;
            cout << "Total second-level slots: " << total_slots << endl;
        }
        
        ~PerfectHashTable() {}
};

int main() 
{
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
    keys.push_back(22.3333);  // Duplicate key for testing
    
    cout << "Creating perfect hash table with keys:";
    for (double key : keys) cout << " " << key;
    cout << endl;
    
    PerfectHashTable table(keys);
    table.printStats();
    
    vector<double> tests;
    //{22.3333, 85.9999, 99.9, 0.0078, -10.5, 0.0002};
    tests.push_back(22.3333);
    tests.push_back(85.9999);
    tests.push_back(99.9);
    tests.push_back(0.0078);
    tests.push_back(-10.5);
    tests.push_back(0.0002);

    cout << "\nTesting lookups:\n";
    for (double key : tests) 
    {
        cout << key << " -> " << (table.lookup(key) ? "Found" : "Not found") << endl;
    }
    
    return 0;
}