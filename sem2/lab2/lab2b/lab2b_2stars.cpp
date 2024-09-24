/*Реалізувати циклічний журнал повідомлень. Задається фіксований розмір журналу N. 
Доступна операція для додавання текстового повідомлення, яка записує повідомлення і час надходження. 
Якщо журнал вже заповнений – новий запис замінює найдавніший. 
Також доступна операція отримання останніх K повідомлень (K<=N).*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
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

    Time () : year(0), month(0), day(0), hour(0), min(0), sec(0) {}

    void getDate()
    {
        do { this -> year = getValidInputInt("Enter year: "); }   while (this -> year < 1);
        do { this -> month = getValidInputInt("Enter month: "); } while (this -> month > 12 || this -> month < 1);
        do { this -> day = getValidInputInt("Enter day: "); }     while (this -> day > daysInMonth(this -> month) || this -> day < 1);
        do { this -> hour = getValidInputInt("Enter hour: "); }   while (this -> hour > 24 || this -> hour < 0);
        do { this -> min = getValidInputInt("Enter minute: "); }  while (this -> min > 59 || this -> min < 0);
        do { this -> sec = getValidInputInt("Enter second: "); }  while (this -> sec > 59 || this -> sec < 0);
    }
};

struct message
{
    string messageText;
    Time time;

    message() : messageText(""), time() {}

    message (string messageText, Time time) : messageText(messageText), time(time) {};
};

struct journal
{
    vector<message> messages;
    size_t journal_size;
    size_t cur;
    size_t count;

    journal (size_t journal_size) 
    {   
        this -> journal_size = journal_size;
        this -> cur = 0;
        this -> count = 0;
        this -> messages.resize(journal_size);
    };

    void add (message newMessage)
    {
        messages[cur % journal_size] = newMessage;
        cur = (cur + 1) % journal_size;

        if (count != journal_size) ++count;
    }

    //операція отримання останніх K повідомлень (K<=N)
    void receiveMessage (size_t K)
    {
        if (K > count) 
        {
            cout << "There are less than " << K << " elements in journal. There are only " << count << " messages: \n";
            K = count;
        }

        for (size_t i = 0; i < K; i++)
        {
            cout << messages[i].messageText;
            cout << setw(15) << messages[i].time.year << "." << messages[i].time.month << "." << messages[i].time.day
            << " " << messages[i].time.hour << ":" << messages[i].time.min << ":" << messages[i].time.sec << endl;
        }
    }

    void receiveMessageBenchmark (size_t K)
    {
        if (K > count) 
        {
            K = count;
        }

        for (size_t i = 0; i < K; i++)
        {
            for(int j = 0; j < 13; j++)
            {}
        }
    }
};

void interactiveMode() 
{
    size_t N;
    cout << "Enter journal size (N): ";
    cin >> N;

    journal jrnl(N);

    int choice;

    do 
    {
        cout << "\nMenu:\n";
        cout << "1. Add message\n";
        cout << "2. Print K messages\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) 
        {
            case 1:
            {
                string msg;
                cout << "\nEnter the message: ";
                cin.ignore(); 
                getline(cin, msg);

                Time date = Time();
                date.getDate();

                message newmsg (msg, date);
                jrnl.add(newmsg);

                break;
            }

            case 2:
            {
                size_t K;
                cout << "Enter the K: ";
                cin >> K;

                jrnl.receiveMessage(K);

                break;
            }

            case 3:
            {
                cout << "Exiting...\n";

                break;
            }

            default:
            cout << "Invalid choice.\n";
            break;
        }

    } while (choice != 3);
}

void demoMode() 
{   
    cout << "N = 3\n";
    journal demoJournal(3);

    message msg1("This is the 1st message :)", Time(2007, 10, 10, 10, 10, 10));
    message msg2("This is the 2nd message :)", Time(2008, 12, 12, 12, 12, 12));
    message msg3("This is the 3rd message :)", Time(2009, 11, 11, 11, 11, 11));
    message msg4("This is the 4th message :)", Time(2010, 11, 11, 11, 11, 11));

    demoJournal.add(msg1);
    demoJournal.add(msg2);
    demoJournal.add(msg3);

    cout << "\nK = 2\n";
    demoJournal.receiveMessage(2);

    cout << "\nK = 4\n";
    demoJournal.receiveMessage(4); 

    cout << "\nAdding 4th message...\n";
    demoJournal.add(msg4);

    cout << "\nK = 3\n";
    demoJournal.receiveMessage(3);
}

void benchmark() 
{
    size_t N, K;
    cout << "\nEnter N: ";
    cin >> N;
    string newMessage[] = {"aa", "bb", "cc", "dd", "ee"};

    journal benchJournal(N);

    auto start = chrono::high_resolution_clock::now();
    auto startAdd = chrono::high_resolution_clock::now();
    for (int j = 0; j < N; j++)
    {
        string text = newMessage[rand() % sizeof(newMessage) / sizeof(newMessage[0])];

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

        Time date(year, month, day, hour, minute, second);

        message benchMessage (text, date);
        benchJournal.add(benchMessage);
    }
    auto endAdd = chrono::high_resolution_clock::now();

    K = N * (0.0001 + rand()%10 / 136.0);
    cout << "Random K: " << K << endl << endl;

    auto startReceive = chrono::high_resolution_clock::now();
    benchJournal.receiveMessageBenchmark(K);
    auto endReceive = chrono::high_resolution_clock::now();

    auto end = chrono::high_resolution_clock::now();

    auto durationAll = chrono::duration_cast<chrono::milliseconds>(end - start);
    auto durationAdd = chrono::duration_cast<chrono::milliseconds>(endAdd - startAdd);
    auto durationReceive = chrono::duration_cast<chrono::milliseconds>(endReceive - startReceive);

    cout << "Time adding random messages: " << durationAdd.count() << endl;
    cout << "Time receiving K messages: " << durationReceive.count() << endl;
    cout << "Duration: " << durationAll.count() << endl;
}

int main () 
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

        default:
        cout << "Invalid choice.\n";
        break;
    }
    return 0;
}
