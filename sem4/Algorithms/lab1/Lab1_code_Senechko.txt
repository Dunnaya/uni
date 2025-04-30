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
                cout << "Generated hash function: a = " << a << ", b = " << b << ", m = " << m << endl;
            }
            
            int operator()(double key) const 
            {
                int64_t k = abs(static_cast<int64_t>(key * 1000000));
                return ((a * k + b) % 1000000007) % m;
            }
        };

        struct Bucket 
        {
            vector<double> data;
            HashFunction* hashFunc;
            bool useSecondHash;
            
            Bucket() : hashFunc(nullptr), useSecondHash(false) {}
            
            Bucket(int size, HashFunction* func) 
                : data(size), hashFunc(func), useSecondHash(true) {}
                
            ~Bucket() { delete hashFunc; }
        };

        vector<vector<double> > firstLevel;
        vector<Bucket> buckets;
        HashFunction firstHash;
        int numKeys;

    public:
        PerfectHashTable(const vector<double>& keys) 
            : firstLevel(keys.size()), buckets(keys.size()), 
            firstHash(keys.size()), numKeys(keys.size()) 
        {
            unordered_set<double> uniqueKeys;
            for (double key : keys) uniqueKeys.insert(key);
            
            cout << "\nFirst level distribution:\n";
            for (double key : uniqueKeys) 
            {
                int idx = firstHash(key);
                firstLevel[idx].push_back(key);
                cout << key << " -> bucket " << idx << endl;
            }
            
            cout << "\nSecond level setup:\n";
            for (int i = 0; i < numKeys; i++) 
            {
                auto& bucket = firstLevel[i];
                if (bucket.empty()) continue;
                
                if (bucket.size() == 1) 
                {
                    buckets[i].data = vector<double>(1, bucket[0]);
                    cout << "Bucket " << i << ": storing " << bucket[0] << " directly\n";
                }
                else 
                {
                    int size = bucket.size() * bucket.size();
                    buckets[i].hashFunc = new HashFunction(size);
                    buckets[i].data.resize(size);
                    buckets[i].useSecondHash = true;
                    
                    cout << "Bucket " << i << ": creating second level table size " 
                        << size << " for " << bucket.size() << " elements\n";
                    
                    bool success = false;
                    while (!success) {
                        success = true;
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
            if (bucket.data.empty()) return false;
            if (!bucket.useSecondHash) return bucket.data[0] == key;
            int secondIdx = (*bucket.hashFunc)(key);
            return (secondIdx < bucket.data.size()) && (bucket.data[secondIdx] == key);
        }
};

int main() 
{
    vector<double> keys;
    keys.push_back(-10.5);
    keys.push_back(22.3333);
    keys.push_back(0.0078);
    keys.push_back(40.1);
    keys.push_back(-52.6666);
    keys.push_back(60.4444);
    keys.push_back(0.0001);
    keys.push_back(85.9999);
    keys.push_back(90.0);
    keys.push_back(22.3333);
    
    cout << "Creating perfect hash table with keys:\n";
    for (double key : keys) cout << " " << key;
    cout << endl;
    
    PerfectHashTable table(keys);
    
    vector<double> tests;
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