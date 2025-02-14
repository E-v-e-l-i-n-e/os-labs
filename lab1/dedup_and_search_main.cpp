#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <sys/time.h>
#include <mutex>
#include <thread>
#include "dedup.h"
#include "searchName.h"


//g++ -O0 -o dedupAndSearch dedup_and_search_main.cpp dedup.cpp searchName.cpp

using namespace std;
mutex mtx;
void threadFunctionDedup(const vector<string>& dedupArgs, int iterCount, vector<long>& durations) {
    long duration = dedup(dedupArgs, iterCount);
    lock_guard<mutex> lock(mtx);
    durations.push_back(duration);
}

void threadFunctionSearch(const vector<string>& searchArgs, int iterCount, vector<long>& durations) {
    long duration = searchName(searchArgs, iterCount);
    lock_guard<mutex> lock(mtx);
    durations.push_back(duration);
}

//  dedupAndSearch.exe C:\\Users\\79124\\Documents\\ElinaComputer Project.gsproj random_numbers256.txt dedup_numbers.txt 2 1
int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    if (argc != 7) {
        lock_guard<mutex> lock(mtx);
        wcout << L"Неправильное количество аргументов" << endl;
        return 1;
    }

    int iterCount;
    int threads_count;
    try {
        iterCount = stoi(argv[5]);
        threads_count = stoi(argv[6]);
    } catch (const exception &e) {
        lock_guard<mutex> lock(mtx);
        wcout << L"Не удалось получить количество итераций или потоков" << endl;
        return 1;
    }

    vector<string> searchArgs;
    searchArgs.push_back(argv [1]);
    searchArgs.emplace_back(argv[2]);
    vector<long> durationsSearch;

    vector<string> dedupArgs;
    dedupArgs.push_back(argv [3]);
    dedupArgs.emplace_back(argv[4]);
    vector<long> durationsDedup;

    vector<thread> threads;
    struct timeval start, end;
    gettimeofday(&start, nullptr);

    for (int i = 0; i < threads_count; i++){
        threads.emplace_back(threadFunctionDedup, dedupArgs, iterCount, ref(durationsDedup));
        threads.emplace_back(threadFunctionSearch, searchArgs, iterCount, ref(durationsSearch));
    }

    for (auto& t : threads) {
        t.join();
    }

    gettimeofday(&end, nullptr);
    long total_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    long avg_duration_search = 0;
    long avg_duration_dedup = 0;

    for (int i = 0; i < durationsSearch.size(); i++){
        avg_duration_search += durationsSearch[i];
    }
    for (int i = 0; i < durationsDedup.size(); i++){
        avg_duration_dedup += durationsDedup[i];
    }
    avg_duration_search /= (durationsSearch.size() * iterCount);
    avg_duration_dedup /= (durationsDedup.size() * iterCount);

    wcout << L"Общее время выполнения программы: " << total_time << L" мс" << endl;
    wcout << L"Среднее время выполнения итерации dedup: " << avg_duration_dedup << L" мс" << endl;
    wcout << L"Среднее время выполнения итерации search name: " << avg_duration_search << L" мс" << endl;

    return 0;
}