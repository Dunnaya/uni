#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>

using namespace std;

class PerfectHash4Real
{
    public:
        struct SecondaryTable
        {
            vector<pair<double, int> > data;
            int size;
            int a, b;

            int universalHash(double key) const
            {
                int intKey = static_cast<int>(round(key * 1e6));
                const int p = 2147483647;
                return ((a * intKey + b) % p) % size;
            }

            void reset()
            {
                data.assign(size, make_pair(numeric_limits<double>::quiet_NaN(), -1));
            }
        };

        vector<SecondaryTable> primaryTable;
        int a, b;

        int primaryHash(double key) const
        {
            int intKey = static_cast<int>(round(key * 1e6));
            const int p = 2147483647;
            return ((a * intKey + b) % p) % primaryTable.size();
        }

        void buildSecondaryTable(vector<double>& keys, int index)
        {
            int n = keys.size();
            if (n == 0) return;

            primaryTable[index].size = n * n;
            primaryTable[index].data.resize(primaryTable[index].size);

            primaryTable[index].a = rand() % 1000 + 1; 
            primaryTable[index].b = rand() % 1000 + 1; 

            for (double key : keys)
            {
                int hash;
                do {
                    hash = primaryTable[index].universalHash(key);
                } while (primaryTable[index].data[hash].first != -1); // Уникаємо колізій

                primaryTable[index].data[hash] = make_pair(key, -1);
            }

            cout << "Secondary table " << index << ": a=" << primaryTable[index].a 
                 << ", b=" << primaryTable[index].b << ", size=" << primaryTable[index].size << endl;
        }

    public:
        PerfectHash4Real(vector<double>& keys)
        {
            int primarySize = max(2, static_cast<int>(sqrt(keys.size())));
            primaryTable.resize(primarySize);

            a = rand() % 1000 + 1; 
            b = rand() % 1000 + 1; 

            cout << "Primary hash: a=" << a << ", b=" << b << ", table size=" << primarySize << endl;

            vector<vector<double> > groups(primarySize);

            for (double key : keys)
            {
                int index = primaryHash(key);
                groups[index].push_back(key);
                cout << "Key " << key << " -> Primary index " << index << endl;
            }

            for (int i = 0; i < primarySize; i++)
            {
                buildSecondaryTable(groups[i], i);
            }
        }

        void setValue(double key, int value)
        {
            int primaryIndex = primaryHash(key);
            auto& table = primaryTable[primaryIndex];

            int secondaryIndex = table.universalHash(key);
            if (secondaryIndex >= 0 && secondaryIndex < table.size && table.data[secondaryIndex].first == key)
            {
                table.data[secondaryIndex].second = value;
                cout << "Stored key " << key << " at [" << primaryIndex << "][" << secondaryIndex << "] with value " << value << endl;
            }
        }

        int getValue(double key) const
        {
            int primaryIndex = primaryHash(key);
            const auto& table = primaryTable[primaryIndex];

            int secondaryIndex = table.universalHash(key);
            if (secondaryIndex >= 0 && secondaryIndex < table.size && table.data[secondaryIndex].first == key)
            {
                cout << "Retrieving key " << key << " from [" << primaryIndex << "][" << secondaryIndex << "] with value " << table.data[secondaryIndex].second << endl;
                return table.data[secondaryIndex].second;
            }

            cout << "Key " << key << " not found in hash table." << endl;
            return -1;
        }

        bool contains(double key) const
        {
            int primaryIndex = primaryHash(key);
            const auto& table = primaryTable[primaryIndex];

            int secondaryIndex = table.universalHash(key);
            bool found = secondaryIndex >= 0 && secondaryIndex < table.size && table.data[secondaryIndex].first == key;
            cout << "Checking if key " << key << " exists: " << (found ? "Yes" : "No") << endl;
            return found;
        }
};
