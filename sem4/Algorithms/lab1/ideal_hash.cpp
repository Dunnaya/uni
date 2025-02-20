#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

const double EMPTY_VALUE = numeric_limits<double>::quiet_NaN();
const int INIT_SIZE = 8;

// Первичная хеш-функция
int primaryHash(double key, int size) 
{
    return static_cast<int>(fabs(key) * 1000000) % size;
}

// Вторичная хеш-функция
int secondaryHash(double key, int size, int seed) 
{
    return (static_cast<int>(fabs(key) * seed) % size);
}

class SecondaryHashTable 
{
    public:
        vector<double> table;
        int seed;

        SecondaryHashTable() : seed(1) {}

        void build(const vector<double>& keys) 
        {
            int n = keys.size();
            table.assign(n * n, EMPTY_VALUE);
            seed = rand() % 100 + 1;
            
            for (double key : keys) 
            {
                int index = secondaryHash(key, table.size(), seed);
                if (!isnan(table[index])) 
                {
                    build(keys); // Перестроить, если есть коллизия
                    return;
                }
                table[index] = key;
            }
        }

        bool search(double key) 
        {
            int index = secondaryHash(key, table.size(), seed);
            return table[index] == key;
        }
};

class PerfectHashTable 
{
    private:
        vector<SecondaryHashTable> table;
        vector<vector<double> > buckets;
        int capacity;

        void rehash() {
            vector<double> keys;
            for (const auto& bucket : buckets) 
            {
                for (double key : bucket) 
                {
                    keys.push_back(key);
                }
            }
            build(keys);
        }

    public:
        PerfectHashTable(int init_size = INIT_SIZE) 
        {
            capacity = init_size;
            table.resize(capacity);
            buckets.resize(capacity);
        }

        void build(const vector<double>& keys) 
        {
            srand(time(nullptr));
            buckets.assign(capacity, vector<double>());
            for (double key : keys) {
                int index = primaryHash(key, capacity);
                buckets[index].push_back(key);
            }
            for (int i = 0; i < capacity; i++) 
            {
                if (!buckets[i].empty()) 
                {
                    table[i].build(buckets[i]);
                }
            }
        }

        void insert(double key) 
        {
            int index = primaryHash(key, capacity);
            buckets[index].push_back(key);
            table[index].build(buckets[index]);
        }

        bool search(double key) 
        {
            int index = primaryHash(key, capacity);
            return table[index].search(key);
        }

        void print() 
        {
            cout << "Perfect Hash Table:" << endl;
            for (int i = 0; i < capacity; i++) 
            {
                cout << "Bucket " << i << ": ";
                for (double key : buckets[i]) 
                {
                    cout << key << " ";
                }
                cout << endl;
            }
        }
};

int main() {
    PerfectHashTable hashTable;
    vector<double> keys;
    //{3.1415, 2.718, 1.618, 0.5772, 1.4142}
    keys.push_back(3.1415);
    keys.push_back(2.718);
    keys.push_back(1.618);
    keys.push_back(0.5772);
    keys.push_back(1.4142);

    hashTable.build(keys);
    
    hashTable.print();
    
    cout << "Search 2.718: " << (hashTable.search(2.718) ? "Found" : "Not Found") << endl;
    cout << "Search 0.999: " << (hashTable.search(0.999) ? "Found" : "Not Found") << endl;
    
    return 0;
}
