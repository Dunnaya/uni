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

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(1, 2147483646);

        bool success = false;
        while (!success)
        {
            primaryTable[index].a = dist(gen);
            primaryTable[index].b = dist(gen);
            primaryTable[index].reset();

            success = true;
            for (double key : keys)
            {
                int hash = primaryTable[index].universalHash(key);
                if (!isnan(primaryTable[index].data[hash].first))
                {
                    success = false;
                    break;
                }
                primaryTable[index].data[hash] = make_pair(key, 0);
            }
        }

        // 🔹 Проверяем, все ли ключи записаны
        for (double key : keys)
        {
            int hash = primaryTable[index].universalHash(key);
            if (isnan(primaryTable[index].data[hash].first) || primaryTable[index].data[hash].first != key)
            {
                cout << "Warning: Key " << key << " not properly stored in secondary table " << index << "!" << endl;
            }
        }

        cout << "Secondary table " << index << ": a=" << primaryTable[index].a
            << ", b=" << primaryTable[index].b << ", size=" << primaryTable[index].size << endl;
    }


public:
    PerfectHash4Real(vector<double>& keys)
    {
        int primarySize = max(2, static_cast<int>(sqrt(keys.size())));
        primaryTable.resize(primarySize);

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(1, 2147483646);

        a = dist(gen);
        b = dist(gen);

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

        // Добавляем проверку isnan() перед сравнением
        if (secondaryIndex >= 0 && secondaryIndex < table.size && 
            !isnan(table.data[secondaryIndex].first) && table.data[secondaryIndex].first == key)
        {
            table.data[secondaryIndex].second = value;
            cout << "Stored key " << key << " at [" << primaryIndex << "][" << secondaryIndex << "] with value " << value << endl;
        }
        else
        {
            cout << "Error: key " << key << " not found in hash table!" << endl;
        }
    }

    int getValue(double key) const
    {
        int primaryIndex = primaryHash(key);
        const auto& table = primaryTable[primaryIndex];

        int secondaryIndex = table.universalHash(key);
        if (secondaryIndex >= 0 && secondaryIndex < table.size && table.data[secondaryIndex].first == key)
        {
            return table.data[secondaryIndex].second;
        }

        return -1;
    }

    bool contains(double key) const
    {
        int primaryIndex = primaryHash(key);
        const auto& table = primaryTable[primaryIndex];

        int secondaryIndex = table.universalHash(key);
        return (secondaryIndex >= 0 && secondaryIndex < table.size && !isnan(table.data[secondaryIndex].first));
    }
};

int main()
{
    vector<double> keys;
    for (int i = 1; i <= 5; i++)
    {
        keys.push_back(i * 1.1);
    }
    keys.push_back(-6.6);

    PerfectHash4Real hashTable(keys);

    hashTable.setValue(1.1, 10);
    hashTable.setValue(2.2, 20);
    hashTable.setValue(3.3, 30);
    hashTable.setValue(4.4, 40);
    hashTable.setValue(5.5, 50);
    hashTable.setValue(-6.6, -60);

    cout << "\nRetrieving values:\n";
    for (double key : keys)
    {
        cout << "Value for key " << key << ": " << hashTable.getValue(key) << endl;
    }

    cout << "\nChecking key presence:\n";
    for (double key : keys)
    {
        cout << "Contains key " << key << ": " << (hashTable.contains(key) ? "Yes" : "No") << endl;
    }

    return 0;
}