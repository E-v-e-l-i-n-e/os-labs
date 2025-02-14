#include "dedup.h"
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <thread>
#include <mutex>

using namespace std;
mutex mtx;
//g++ -O0 -o dedup dedup_main.cpp dedup.cpp

void threadFunction(const vector<string>& dedupArgs, int iterCount, vector<long>& durations) {
    long duration = dedup(dedupArgs, iterCount);
    lock_guard<mutex> lock(mtx);
    durations.push_back(duration);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    if (argc != 5) {
        lock_guard<mutex> lock(mtx);
        wcout << L"Неправильное количество аргументов" << endl;
        return 1;
    }
    int iterCount;
    int threads_count;
    try {
        iterCount = stoi(argv[3]);
        threads_count = stoi(argv[4]);
    } catch (const exception &e) {
        lock_guard<mutex> lock(mtx);
        wcout << L"Не удалось получить количество итераций или потоков" << endl;
        return 1;
    }

    vector<string> dedupArgs;
    dedupArgs.push_back(argv [1]);
    dedupArgs.emplace_back(argv[2]);
    vector<long> durations;
    vector<thread> threads;

    struct timeval start, end;
    gettimeofday(&start, nullptr);

    for (int i = 0; i < threads_count; i++){
        threads.emplace_back(threadFunction, dedupArgs, iterCount, ref(durations));
    }

    for (auto& t : threads) {
        t.join();
    }

    gettimeofday(&end, nullptr);
    long total_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    long avg_duration = 0;

    for (int i = 0; i < durations.size(); i++){
        avg_duration += durations[i];
    }

    avg_duration /= (durations.size() * iterCount);

    wcout << L"Общее время выполнения программы: " << total_time << L" мс" << endl;
    wcout << L"Среднее время выполнения итерации: " << avg_duration << L" мс" << endl;

    return 0;
}