/*Змоделювати поведінку планувальника задач (job scheduler). На вхід подаються задачі, 
для яких відомий час виконання і час, коли вони надходять. Планувальник ставить задачі 
до черги, потім один чи кілька обробників забирають задачі з черги і виконують їх. Реалізувати 
схеми роботи First Come First Served (FCFS) – задачі виконуються в порядку надходження, та 
Shortest Job First (SJF) – першою виконується задача з найменшим часом виконання.*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <thread>
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

    Time (int hour, int min, int sec) 
        : year(0), month(0), day(0), hour(hour), min(min), sec(sec) {}

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

    void getDateJob()
    {
        do { this -> hour = getValidInputInt("Enter hour: "); }   while (this -> hour > 24 || this -> hour < 0);
        do { this -> min = getValidInputInt("Enter minute: "); }  while (this -> min > 59 || this -> min < 0);
        do { this -> sec = getValidInputInt("Enter second: "); }  while (this -> sec > 59 || this -> sec < 0);
    }

    int countSeconds()
    {
        int dayz = 0;
        int secs = 0;

        for (int year = 0; year < this -> year; ++year)
        {
            dayz += (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 366 : 365;
        }

        for (int month = 1; month < this -> month; month++)
        {
            dayz += daysInMonth(month);
        }

        dayz += this -> day;

        secs = dayz * 24 * 3600 + this -> hour * 3600 + this -> min * 60 + this -> sec;

        return secs;
    }
};

struct job
{
    string text;
    Time arrive;
    Time duration;

    int seconds = duration.hour * 3600 + duration.min * 60 + duration.sec;

    job (string text, Time arrive, Time duration) : text(text), arrive(arrive), duration(duration) {};
    job () {};
};

struct scheduler
{
    vector<job> jobs;

    void add(job newJob)
    {
        jobs.push_back(newJob);
    }

    void shortestJobSort() //bubble sort
    {
        for(int i = 0; i < (jobs.size() - 1); i++)
        {
            for(int j = 0; j < (jobs.size() - 1); j++)
            {
                if(jobs[j].seconds < jobs[j+1].seconds)
                {
                    job temp = jobs[j];
                    jobs[j] = jobs[j+1];
                    jobs[j+1] = temp;
                }
            }
        }
    }

    void firstComeFirstServed()
    {
        for(int i = 0; i < (jobs.size() - 1); i++)
        {
            for(int j = 0; j < (jobs.size() - 1); j++)
            {
                if(jobs[j].arrive.countSeconds() < jobs[j+1].arrive.countSeconds())
                {
                    job temp = jobs[j];
                    jobs[j] = jobs[j+1];
                    jobs[j+1] = temp;
                }
            }
        }
    }

    void schedulerWork()
    {
        for(int i = jobs.size() - 1; i >= 0; i--)
        {
            cout << "\nCompleting " << jobs[i].text << "...\n";
            this_thread::sleep_for(chrono::milliseconds(jobs[i].seconds));
            cout << "Done!\n";
            jobs.pop_back();
        }
    }

    void schedulerWorkBenchmark()
    {
        for(int i = jobs.size() - 1; i >= 0; i--)
        {
            this_thread::sleep_for(chrono::milliseconds(jobs[i].seconds));
            jobs.pop_back();
        }
    }

    scheduler () {};
};

void interactiveMode() 
{
    scheduler schdlr;

    int choice;

    do 
    {
        cout << "\nMenu:\n";
        cout << "1. Add task\n";
        cout << "2. Start FCFS\n";
        cout << "3. Start SJF\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) 
        {
            case 1:
            {
                string text;
                cout << "\nEnter the task: ";
                cin.ignore(); 
                getline(cin, text);

                Time time_arrive = Time();
                time_arrive.getDate();

                cout << "\nEnter completion time: " << endl;
                Time duration_time = Time();
                duration_time.getDateJob();

                job newJobs (text, time_arrive, duration_time);
                schdlr.add(newJobs);

                break;
            }

            case 2:
            {
                schdlr.firstComeFirstServed();
                schdlr.schedulerWork();

                break;
            }

            case 3:
            {
                schdlr.shortestJobSort();
                schdlr.schedulerWork();

                break;
            }

            case 4:
            {
                cout << "Exiting...\n";

                break;
            }

            default:
            cout << "Invalid choice.\n";
            break;
        }

    } while (choice != 4);
}

void demoMode() 
{
    scheduler demoScheduler;

    job task1("the 1st task", Time(2007, 10, 10, 10, 10, 10), Time(2, 30, 10));
    job task2("the 2nd task", Time(2008, 12, 12, 12, 12, 12), Time(0, 25, 11));
    job task3("the 3rd task", Time(2009, 11, 11, 11, 11, 11), Time(3, 10, 11));
    job task4("the 4th task", Time(2010, 11, 11, 11, 11, 11), Time(7, 11, 11));
    job task5("the 5th task", Time(2011, 11, 11, 11, 11, 11), Time(0, 0, 30));

    cout << "\nAdding 5 tasks...\n";
    demoScheduler.add(task1);
    demoScheduler.add(task2);
    demoScheduler.add(task3);
    demoScheduler.add(task4);
    demoScheduler.add(task5);

    cout << "\n\tFCFS:\n";
    demoScheduler.firstComeFirstServed();
    demoScheduler.schedulerWork();

    cout << "\nAdding 5 tasks again...\n";
    demoScheduler.add(task1);
    demoScheduler.add(task2);
    demoScheduler.add(task3);
    demoScheduler.add(task4);
    demoScheduler.add(task5);

    cout << "\n\tSJF:\n";
    demoScheduler.shortestJobSort();
    demoScheduler.schedulerWork();
}

void benchmark() 
{
    int N;
    cout << "\nEnter N: ";
    cin >> N;
    string newTask[] = {"aa", "bb", "cc", "dd", "ee"};

    scheduler benchScheduler;

    auto start = chrono::high_resolution_clock::now();
    auto startAdd = chrono::high_resolution_clock::now();
    for (int j = 0; j < N; j++)
    {
        string text = newTask[rand() % sizeof(newTask) / sizeof(newTask[0])];

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

        Time benchArrive(year, month, day, hour, minute, second);

        hour = 1 + (rand()% 5);
        minute = 1 + (rand()% 60);
        second = 1 + (rand()% 60);

        Time benchDuration(hour, minute, second);

        job benchTask (text, benchArrive, benchDuration);
        benchScheduler.add(benchTask);
    }
    auto endAdd = chrono::high_resolution_clock::now();

    auto startFCFSsort = chrono::high_resolution_clock::now();
    benchScheduler.firstComeFirstServed();
    auto endFCFSsort = chrono::high_resolution_clock::now();

    auto startFCFSwork = chrono::high_resolution_clock::now();
    benchScheduler.schedulerWorkBenchmark();
    auto endFCFSwork = chrono::high_resolution_clock::now();



    auto startAddAgain = chrono::high_resolution_clock::now();
    for (int j = 0; j < N; j++)
    {
        string text = newTask[rand() % sizeof(newTask) / sizeof(newTask[0])];

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

        Time benchArrive(year, month, day, hour, minute, second);

        hour = 1 + (rand()% 5);
        minute = 1 + (rand()% 60);
        second = 1 + (rand()% 60);

        Time benchDuration(hour, minute, second);

        job benchTask (text, benchArrive, benchDuration);
        benchScheduler.add(benchTask);
    }
    auto endAddAgain = chrono::high_resolution_clock::now();

    auto startSJFsort = chrono::high_resolution_clock::now();
    benchScheduler.shortestJobSort();
    auto endSJFsort = chrono::high_resolution_clock::now();

    auto startSJFwork = chrono::high_resolution_clock::now();
    benchScheduler.schedulerWorkBenchmark();
    auto endSJFwork = chrono::high_resolution_clock::now();
    
    auto end = chrono::high_resolution_clock::now();

    auto durationAll = chrono::duration_cast<chrono::milliseconds>(end - start);
    auto durationAddFirst = chrono::duration_cast<chrono::milliseconds>(endAdd - startAdd);
    auto durationFCFSsort = chrono::duration_cast<chrono::milliseconds>(endFCFSsort - startFCFSsort);
    auto durationFCFSwork = chrono::duration_cast<chrono::milliseconds>(endFCFSwork - startFCFSwork);
    auto durationAddSecond = chrono::duration_cast<chrono::milliseconds>(endAddAgain - startAddAgain);
    auto durationSJFsort = chrono::duration_cast<chrono::milliseconds>(endSJFsort - startSJFsort);
    auto durationSJFwork = chrono::duration_cast<chrono::milliseconds>(endSJFwork - startSJFwork);

    cout << "Time adding random tasks: " << durationAddFirst.count() << endl;
    cout << "Time sorting for FCFS: " << durationFCFSsort.count() << endl;
    cout << "Time FCFS working: " << durationFCFSwork.count() << endl;
    cout << "Time adding random tasks (again): " << durationAddSecond.count() << endl;
    cout << "Time sorting for SJF: " << durationSJFsort.count() << endl;
    cout << "Time SJF working: " << durationSJFwork.count() << endl;
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
