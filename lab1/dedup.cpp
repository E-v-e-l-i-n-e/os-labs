#include "dedup.h"
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <sys/time.h>
#include <fcntl.h>
#include <mutex>

using namespace std;

const size_t MEMORY_LIMIT = 32 * 1024 * 1024; // 32 МБ

long dedup(const vector<string>& args, int repetitions) {
    if (args.size() != 2) {
        lock_guard<mutex> lock(mtx);
        wcout << L"Неправильные входные данные" << endl;
        exit(1);
    }
    const int numThreads = repetitions;
    string inputFile = args[0];
    string outputFile = args[1];

    struct timeval start, end;
    gettimeofday(&start, nullptr);
    for (int i = 0; i < repetitions; ++i) {
        int inFile = open(inputFile.c_str(), O_RDONLY);
        int outFile = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (inFile == -1 || outFile == -1) {
            lock_guard<mutex> lock(mtx);
            wcout << L"Ошибка. Не удалось найти файл" << endl;
          exit(1);
        }

        unordered_set<string> uniqueLines;
        vector<char> buffer(MEMORY_LIMIT);

        ssize_t bytesRead;
        while ((bytesRead = read(inFile, buffer.data(), buffer.size())) > 0) {
            size_t pos = 0;
            while (pos < bytesRead) {

                char* end = static_cast<char*>(memchr(buffer.data() + pos, '\n', bytesRead - pos));
                if (!end) {
                    end = buffer.data() + bytesRead;
                }

                string line(buffer.data() + pos, end - (buffer.data() + pos));
                if (uniqueLines.find(line) == uniqueLines.end()) {
                    uniqueLines.insert(line);
                    write(outFile, line.c_str(), line.size());
                    write(outFile, "\n", 1);
                }
                pos = end - buffer.data() + 1;
            }
        }
        close(inFile);
        close(outFile);
        lock_guard<mutex> lock(mtx);
//        wcout << L"Дедупликация завершена (итерация " << i + 1 << ")" << endl;
    }
    gettimeofday(&end, nullptr);
    long duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    return duration;
}