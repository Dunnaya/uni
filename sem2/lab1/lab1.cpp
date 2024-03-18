/* 2. Інформація про товари в магазині. Зберігаються наступні дані про кожен товар: назва товару; 
одиниці виміру – кілограми, літри, поштучно, пакунки; кількість товару (у відповідних одиницях, дійсне число); дата та час виробництва; термін зберігання (кількість 
діб з дати виробництва, не більше 10 років). Критерії пошуку:
b. Товари з назвою, що містить заданий фрагмент тексту; товари з кількістю не менше заданої; 
товари, термін зберігання яких спливає до заданого дня.*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <chrono>

using namespace std;

int getValidInputInt (const string& input) 
{
    int value;
    bool cinFail;

    do
    {
        cinFail = false;
        cout << input;
        cin >> value;

        if (cin.fail()) 
        {
            cerr << "\n\t\tInvalid input.\n\n";
            cin.clear();
            cin.ignore(numeric_limits <streamsize>::max(), '\n');
            cinFail = true;
        };
        
    } while (cinFail);

    return value;
}

struct Time 
{
    int daysInMonth(int month)
    {
        switch (month)
        {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) return 29; else return 28;
        default:
            return -1;
        }
    }

    int year, month, day, hour, min, sec;

    Time (int year, int month, int day, int hour, int min, int sec) 
        : year(year), month(month), day(day), hour(hour), min(min), sec(sec) {}

    Time () {}

    void getDate()
    {
        do { this -> year = getValidInputInt("Enter year: "); }   while (this -> year < 1);
        do { this -> month = getValidInputInt("Enter month: "); } while (this -> month > 12 || this -> month < 1);
        do { this -> day = getValidInputInt("Enter day: "); }     while (this -> day > daysInMonth(this -> month) || this -> day < 1);
        do { this -> hour = getValidInputInt("Enter hour: "); }   while (this -> hour > 24 || this -> hour < 0);
        do { this -> min = getValidInputInt("Enter minute: "); }  while (this -> min > 59 || this -> min < 0);
        do { this -> sec = getValidInputInt("Enter second: "); }  while (this -> sec > 59 || this -> sec < 0);
    }

    int countDays()
    {
        int dayz = 0;

        for (int year = 2013; year < this -> year; ++year)
        {
            dayz += (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 366 : 365;
        }

        for (int month = 1; month < this -> month; month++)
        {
            dayz += daysInMonth(month);
        }

        dayz += this -> day;

        return dayz;
    }
};

struct Product 
{
    string name;
    string units;
    double quantity;
    Time production_date;
    int storage_period;

    Product(string name, string units, double quantity, Time production_date, int storage_period)
            : name(name), units(units), quantity(quantity), production_date(production_date), storage_period(storage_period) {}

    Product(){};
};

struct Store 
{
    vector<Product> products;

    void add_product(Product product) 
    {
        products.push_back(product);
    }

    void print_all_products() 
    {
        cout << "\nProducts in the store:\n";
        cout << setw(20) << "Name" << setw(10) << "Units" << setw(15) << "Quantity" << setw(25) << "Production Date" << endl;

        for (auto& product : products) 
        {
            cout << setw(20) << product.name << setw(10) << product.units << setw(15) << product.quantity
                 << setw(10) << product.production_date.year << "." << product.production_date.month << "." << product.production_date.day
                 << " " << product.production_date.hour << ":" << product.production_date.min << ":" << product.production_date.sec << endl;
        }
    }

    //товари з кількістю не менше заданої
    void searchQuantity(double quantity)
    {
        cout << "Products that match the search criteria: \n";

        bool flag = false;

        for (int i = 0; i < products.size(); i++) 
        {
            if (products[i].quantity >= quantity) 
            {
                flag = true;
                cout << products[i].name << endl;
            }
        }

        if (flag == false) cout << "None.\n";
    }

    void searchQuantityBenchmark(double quantity)
    {
        int a = 0;

        bool flag = false;

        for (int i = 0; i < products.size(); i++) 
        {
            if (products[i].quantity >= quantity) 
            {
                flag = true;
                a += 1;
            }
        }
    }

    //товари з назвою, що містить заданий фрагмент тексту
    void searchName(string name)
    {
        cout << "Products that match the search criteria: \n";

        bool flag = false;

        for (int i = 0; i < products.size(); i++) 
        {
            if (products[i].name.find(name) != string::npos) 
            {
                flag = true;
                cout << products[i].name << endl;
            }
        }

        if (flag == false) cout << "None.\n";
    }

    void searchNameBenchmark(string name)
    {
        int a = 0;

        bool flag = false;

        for (int i = 0; i < products.size(); i++) 
        {
            if (products[i].name.find(name) != string::npos) 
            {
                flag = true;
                a += 1;
            }
        }
    }

    //товари, термін зберігання яких спливає до заданого дня
    void searchDate(Time newDate)
    {
        cout << "Products that match the search criteria: \n";

        bool flag = false;

        for (int i = 0; i < products.size(); i++)
        {
            if (products[i].production_date.countDays() + products[i].storage_period < newDate.countDays())
            {
                flag = true;
                cout << products[i].name << endl;
            }
        }

        if (flag == false) cout << "None.\n";
    }

    void searchDateBenchmark(Time newDate)
    {
        int a = 0;

        bool flag = false;

        for (int i = 0; i < products.size(); i++)
        {
            if (products[i].production_date.countDays() + products[i].storage_period < newDate.countDays())
            {
                flag = true;
                a += 1;
            }
        }
    }
};

void writeBin(const Store& store)
{
    ofstream binfile("database.bin", ios::binary | ios::out);

    binfile << store.products.size() << "\n";

    for (auto item: store.products)
    {
        binfile << item.name << " " << item.units << " " << item.quantity << " " << item.storage_period << " "
         << item.production_date.year << " " << item.production_date.month << " " << item.production_date.day << " "
         << item.production_date.hour << " " << item.production_date.min << " " << item.production_date.sec << "\n";
    }

    binfile.close();

    cout << "I'm done writing!\n";
}

void writeBinBenchmark(const Store& store)
{
    ofstream binfile("database.bin", ios::binary | ios::out);

    binfile << store.products.size() << "\n";

    for (auto item: store.products)
    {
        binfile << item.name << " " << item.units << " " << item.quantity << " " << item.storage_period << " "
         << item.production_date.year << " " << item.production_date.month << " " << item.production_date.day << " "
         << item.production_date.hour << " " << item.production_date.min << " " << item.production_date.sec << "\n";
    }

    binfile.close();
}

int readBin(Store& store)
{
    string path;
    cout << "Enter the path: ";
    cin >> path;

    ifstream binfile(path, ios::binary| ios::in);

    if (!binfile.is_open())
    {
        cerr << "Could not open file for reading\n";
        return 1;
    }

    int n; //кількість елементів у файлі (продуктів)
    binfile >> n;
    
    for (int i = 0; i < n; i++)
    {
        Product item;

        binfile >> item.name >> item.units >> item.quantity >> item.storage_period
         >> item.production_date.year >> item.production_date.month >> item.production_date.day
         >> item.production_date.hour >> item.production_date.min >> item.production_date.sec;
         store.products.push_back(item);
    }

    binfile.close();
    cout << "I'm done writing!\n";

    return 0;
}

int readBinBenchmark(Store& store)
{
    ifstream binfile("database.bin", ios::binary| ios::in);

    if (!binfile.is_open())
    {
        cerr << "Could not open file for reading\n";
        return 1;
    }

    int n; //кількість елементів у файлі (продуктів)
    binfile >> n;
    
    for (int i = 0; i < n; i++)
    {
        Product item;

        binfile >> item.name >> item.units >> item.quantity >> item.storage_period
         >> item.production_date.year >> item.production_date.month >> item.production_date.day
         >> item.production_date.hour >> item.production_date.min >> item.production_date.sec;
         store.products.push_back(item);
    }

    binfile.close();

    return 0;
}

void writeTxt(const Store& store)
{
    ofstream txtfile("database.txt", ios::out);

    for (auto item: store.products)
    {
        txtfile << item.name << " " << item.units << " " << item.quantity << " " << item.storage_period << " "
         << item.production_date.year << " " << item.production_date.month << " " << item.production_date.day << " "
         << item.production_date.hour << " " << item.production_date.min << " " << item.production_date.sec << "\n";
    }

    txtfile.close();

    cout << "I'm done writing!\n";
}

void writeTxtBenchmark(const Store& store)
{
    ofstream txtfile("database.txt", ios::out);

    for (auto item: store.products)
    {
        txtfile << item.name << " " << item.units << " " << item.quantity << " " << item.storage_period << " "
         << item.production_date.year << " " << item.production_date.month << " " << item.production_date.day << " "
         << item.production_date.hour << " " << item.production_date.min << " " << item.production_date.sec << "\n";
    }

    txtfile.close();
}

int readTxt(Store& store)
{
    string path;
    cout << "Enter the path: ";
    cin >> path;

    ifstream txtfile(path, ios::in);

    if (!txtfile.is_open())
    {
        cerr << "Could not open file for reading\n";
        return 1;
    }
    
    while (txtfile.peek() != EOF) 
    {
        Product item;

        txtfile >> item.name >> item.units >> item.quantity >> item.storage_period
         >> item.production_date.year >> item.production_date.month >> item.production_date.day
         >> item.production_date.hour >> item.production_date.min >> item.production_date.sec;
         store.products.push_back(item);
    }

    txtfile.close();
    cout << "I'm done reading!\n";

    return 0;
}

int readTxtBenchmark(Store& store)
{
    ifstream txtfile("database.txt", ios::in);

    if (!txtfile.is_open())
    {
        cerr << "Could not open file for reading\n";
        return 1;
    }
    
    while (!txtfile.eof()) 
    {
        Product item;

        txtfile >> item.name >> item.units >> item.quantity >> item.storage_period
         >> item.production_date.year >> item.production_date.month >> item.production_date.day
         >> item.production_date.hour >> item.production_date.min >> item.production_date.sec;
         store.products.push_back(item);
    }

    txtfile.close();

    return 0;
}

void interactiveMode()
{
    Store store;

    int choice;

    do 
    {
        cout << "Menu:\n";
        cout << "1. Add Product\n";
        cout << "2. Print All Products\n";
        cout << "3. Search\n";
        cout << "4. Write file (txt/bin)\n";
        cout << "5. Read file (txt/bin)\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) 
        {
            case 1: 
            {
                string name, units;
                double quantity;
                int storage_period;

                cout << "Enter product name: ";
                cin >> name;
                cout << "Enter units: ";
                cin >> units;
                cout << "Enter quantity: ";
                cin >> quantity;
                cout << "Enter storage period (in days): ";
                cin >> storage_period;

                Time production_date = Time();
                production_date.getDate();

                Product product(name, units, quantity, production_date, storage_period);
                store.add_product(product);

                cout << "Product added successfully!\n";

                break;
            }

            case 2: 
            {
                store.print_all_products();

                break;
            }
            
            case 3:
            {
                int searchChoice;

                cout << "Available search criteria: \n";
                cout << "1. Quantity\n";
                cout << "2. Name\n";
                cout << "3. Expiration Date\n";
                cout << "Select search criteria: ";

                cin >> searchChoice;

                switch (searchChoice)
                {
                    case 1:
                    {
                        int quant;

                        cout << "Enter quantity: ";
                        cin >> quant;
                        store.searchQuantity(quant);

                        break;
                    }

                    case 2:
                    {
                        string fragment;

                        cout << "Enter text fragment: ";
                        cin >> fragment;
                        store.searchName(fragment);

                        break;
                    }

                    case 3:
                    {
                        Time newNewDate;
                        newNewDate.getDate();
                        store.searchDate(newNewDate);

                        break;
                    }

                    default:
                        cout << "Invalid choice.\n";
                }

                break;
            }

            default:
                cout << "Invalid choice.\n";

            case 4:
            {
                int dataType;

                cout << "Available data types:\n";
                cout << "1. Bin\n";
                cout << "2. Txt\n";
                cout << "Select data type: ";
                cin >> dataType;

                switch (dataType)
                {
                    case 1:
                    {
                        writeBin(store);

                        break;
                    }

                    case 2: 
                    {
                        writeTxt(store);
                        
                        break;
                    }

                    default:
                    cout << "Invalid choice.\n";
                }
                break;
            }

            case 5:
            {
                int dataType;

                cout << "Available data types:\n";
                cout << "1. Bin\n";
                cout << "2. Txt\n";
                cout << "Select data type: ";
                cin >> dataType;

                switch (dataType)
                {
                    case 1:
                    {
                        readBin(store);

                        break;
                    }

                    case 2: 
                    {
                        readTxt(store);

                        break;
                    }

                    default:
                    cout << "Invalid choice.\n";
                }
                break;
            }

            case 6:
                cout << "Exiting...\n";
                break;
        }

    } while (choice != 6);
}

void demoMode()
{
    Store store;

    Product product1("Milk", "l",   10.5, Time(2022, 10, 10, 10, 10, 10), 7);
    Product product2("Cookies", "kg",  20.3, Time(2022, 12, 12, 12, 12, 12), 45);
    Product product3("Buns", "pcs", 15, Time(2022, 11, 11, 11, 11, 11), 14);

    store.add_product(product1);
    store.add_product(product2);
    store.add_product(product3);

    store.print_all_products();

    cout << "\nStorage period for " << product1.name << " is " << product1.storage_period << " days\n";
    cout << "Storage period for " << product2.name << " is " << product2.storage_period << " days\n";
    cout << "Storage period for " << product3.name << " is " << product3.storage_period << " days\n";

    cout << "\nSearch for products with quantity >= 15:\n";
    store.searchQuantity(15);

    cout << "\nSearch for products with names containing 'ok':\n";
    store.searchName("ok");

    cout << "\nSearch for products expiring before 2022-12-15:\n";
    Time expirationDate(2022, 12, 15, 0, 0, 0);
    store.searchDate(expirationDate);
}

void benchmark()
{
    int n;
    cout << "\nEnter N: ";
    cin >> n;

    string product[] = {"aa", "bb", "cc", "dd", "ee"};
    string units[] = {"kg", "l", "pcs", "packs"};

    Store store;

    int total = 0;

    for(int i = 0; i <3; i++)
    {
        auto start = chrono::high_resolution_clock::now();
        auto startAdd = chrono::high_resolution_clock::now();
        for (int j = 0; j < n; j++)
        {
            string name = product[rand() % sizeof(product) / sizeof(product[0])];
            string unit = units[rand() % sizeof(units) / sizeof(units[0])];
            double quantity = rand()% 1000 / 100.0;
            int year = 2014 + (rand()% 11);
            int month = 1 + (rand()% 12);
            int day = 1;

            switch (month)
            {
                case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                    {
                        day = 1 + (rand()% 31);
                        break;
                    };
                case 4: case 6: case 9: case 11:
                    {
                        day = 1 + (rand()% 30);
                        break;
                    }
                case 2:
                    {
                        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) 
                        day = 1 + (rand()% 29); 
                        else 
                        day = 1 + (rand()% 28);
                        break;
                    }
            }

            int hour = 1 + (rand()% 24);
            int minute = 1 + (rand()% 60);
            int second = 1 + (rand()% 60);

            int storagePeriod = 1 + (rand()% 150);

            Time productionDate(year, month, day, hour, minute, second);

            Product productt(name, unit, quantity, productionDate, storagePeriod);

            store.add_product(productt);
        }
        auto endAdd = chrono::high_resolution_clock::now();

        if (i == 0)
        {
            auto startWrite = chrono::high_resolution_clock::now();
            writeBinBenchmark(store);
            auto endWrite = chrono::high_resolution_clock::now();

            store.products.clear();

            auto startRead = chrono::high_resolution_clock::now();
            readBinBenchmark(store);
            auto endRead = chrono::high_resolution_clock::now();

            auto durationWrite = chrono::duration_cast<chrono::milliseconds>(endWrite - startWrite);
            auto durationRead = chrono::duration_cast<chrono::milliseconds>(endRead - startRead);

            cout << "\nTime using BIN\n";
            cout << "Writing time: " << durationWrite.count() << endl;
            cout << "Reading time: " << durationRead.count() << endl;
        }

        if (i == 1)
        {
            auto startWrite = chrono::high_resolution_clock::now();
            writeTxtBenchmark(store);
            auto endWrite = chrono::high_resolution_clock::now();

            store.products.clear();

            auto startRead = chrono::high_resolution_clock::now();
            readTxtBenchmark(store);
            auto endRead = chrono::high_resolution_clock::now();

            auto durationWrite = chrono::duration_cast<chrono::milliseconds>(endWrite - startWrite);
            auto durationRead = chrono::duration_cast<chrono::milliseconds>(endRead - startRead);

            cout << "\nTime using TXT\n";
            cout << "Writing time: " << durationWrite.count() << endl;
            cout << "Reading time: " << durationRead.count() << endl;
        }

        auto startSearchAll = chrono::high_resolution_clock::now();

        auto startSearchQuantity = chrono::high_resolution_clock::now();
        store.searchQuantityBenchmark(15);
        auto endSearchQuantity = chrono::high_resolution_clock::now();

        auto startSearchName = chrono::high_resolution_clock::now();
        store.searchNameBenchmark("ok");
        auto endSearchName = chrono::high_resolution_clock::now();

        auto startSearchDate = chrono::high_resolution_clock::now();
        Time expirationDate(2022, 12, 15, 0, 0, 0);
        store.searchDateBenchmark(expirationDate);
        auto endSearchDate = chrono::high_resolution_clock::now();

        auto endSearchAll = chrono::high_resolution_clock::now();

        auto end = chrono::high_resolution_clock::now();

        auto durationAll = chrono::duration_cast<chrono::milliseconds>(end - start);
        auto durationAdd = chrono::duration_cast<chrono::milliseconds>(endAdd - startAdd);
        auto durationSearchName = chrono::duration_cast<chrono::milliseconds>(endSearchName - startSearchName);
        auto durationSearchQuantity = chrono::duration_cast<chrono::milliseconds>(endSearchQuantity - startSearchQuantity);
        auto durationSearchDate = chrono::duration_cast<chrono::milliseconds>(endSearchDate - startSearchDate);
        auto durationSearchAll = chrono::duration_cast<chrono::milliseconds>(endSearchAll - startSearchAll);

        if (i == 2)
        cout << "\nTime using VECTOR\n";

        cout << "Time adding products: " << durationAdd.count() << endl;
        cout << "Time searching name: " << durationSearchName.count() << endl;
        cout << "Time searching quantity: " << durationSearchQuantity.count() << endl;
        cout << "Time searching date: " << durationSearchDate.count() << endl;
        cout << "Time searching all: " << durationSearchAll.count() << endl;
        cout << "Duration: " << durationAll.count() << endl;

        total += durationAll.count();

        store.products.clear();
    }

    cout << "\nTotal time: " << total;
}

int main() 
{   
    int choice;

    cout << "Modes:\n";
    cout << "1. Interactive\n" ;
    cout << "2. Demo\n";
    cout << "3. Benchmark\n";
    cout << "Select mode: ";
    cin >> choice;

    switch(choice)
    {
        case 1:
        {
            interactiveMode();

            break;
        }

        case 2:
        {
            demoMode();

            break;
        }

        case 3:
        {
            benchmark();

            break;
        }
    }
    return 0;
}
