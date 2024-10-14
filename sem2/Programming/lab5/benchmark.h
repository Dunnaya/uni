#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <string>
#include <iostream>
#include <thread>
#include <cstddef>
using namespace std;

size_t get_current_virtual_memory();
size_t get_current_physical_memory();

struct Benchmark 
{
  size_t start_virtual_memory;
  size_t start_physical_memory;
  string name;
  Benchmark(string name): start_virtual_memory(get_current_virtual_memory()),
      start_physical_memory(get_current_physical_memory()),
      name(name) {}

  ~Benchmark() 
  {
    size_t end_virtual_memory = get_current_virtual_memory();
    size_t end_physical_memory = get_current_physical_memory();

    cout<<"Benchmark " << name << ": virtual " << end_virtual_memory - start_virtual_memory <<
        " bytes, physical " << end_physical_memory - start_physical_memory << " bytes\n";
  }
};

inline long long memory_difference(size_t start, size_t end) 
{
    if (end >= start)
        return end-start;
    else
        return -(start-end);
}

struct BenchmarkMax 
{
  size_t start_virtual_memory;
  size_t start_physical_memory;
  string name;
  size_t max_virtual_memory;
  size_t max_physical_memory;
  bool is_running;
  thread measurement_thread;

  BenchmarkMax(string name):
      start_virtual_memory(get_current_virtual_memory()),
      start_physical_memory(get_current_physical_memory()),
      name(name),
      max_virtual_memory(0),
      max_physical_memory(0),
      is_running(true),
      measurement_thread(&BenchmarkMax::run, this)
      { }


  void measure() 
  {
    size_t current_virtual_memory = get_current_virtual_memory();
    if (current_virtual_memory > max_virtual_memory)
        max_virtual_memory = current_virtual_memory;

    size_t current_physical_memory = get_current_physical_memory();
    if (current_physical_memory > max_physical_memory)
        max_physical_memory = current_physical_memory;
  }

  void run() 
  {
    do {
      measure();
    } while(is_running);
  }

  ~BenchmarkMax() 
  {
    is_running = false;
    measurement_thread.join();

    cout << "BenchmarkMax " << name << ": virtual " << memory_difference(start_virtual_memory, max_virtual_memory) <<
        " bytes, physical " << memory_difference(start_physical_memory, max_physical_memory) << " bytes\n";
  }
};

#endif
