#include "dedup.h"

#include "dedup.h"
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <sys/time.h>
#include <fcntl.h>
#include "random_cache.h"

using namespace std;

const size_t MEMORY_LIMIT = 32 * 1024 * 1024; // 32 МБ
off_t globalOffset = 0;

off_t globalOffsetInFile = 0;
off_t outOffset = 0;

long dedup(const vector<string>& args) {
    setlocale(LC_ALL, "");
    if (args.size() != 2) {
        wcout << L"Неправильные входные данные" << endl;
        exit(1);
    }
    string inputFile = args[0];
    string outputFile = args[1];

    struct timeval start, end;
    gettimeofday(&start, nullptr);
    int inFile = open(inputFile.c_str(), O_RDONLY);
    int outFile = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (inFile == -1 || outFile == -1) {
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
    wcout << L"Дедупликация завершена" << endl;

    gettimeofday(&end, nullptr);
    long duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    return duration;
}



long dedup_with_cache(const vector<string>& args) {
    setlocale(LC_ALL, "");
    if (args.size() != 2) {
        wcout << L"Неправильные входные данные" << endl;
        exit(1);
    }

    string inputFile = args[0];
    string outputFile = args[1];

    struct timeval start, end;
    gettimeofday(&start, nullptr);

    int inFile = lab2_open(inputFile.c_str());
    int outFile = lab2_open(outputFile.c_str());

    if (inFile == -1 || outFile == -1) {
        wcout << L"Ошибка. Не удалось найти файл" << endl;
        exit(1);
    }

    unordered_set<string> uniqueLines;
    vector<char> buffer(MEMORY_LIMIT);
    ssize_t bytesRead;
    wcout << L"Начинается выполнение алгоритма" << endl;
    while ((bytesRead = lab2_read(inFile, buffer.data(), buffer.size(), globalOffset)) > 0) {
        size_t pos = 0;
        while (pos < bytesRead) {
            char* end = static_cast<char*>(memchr(buffer.data() + pos, '\n', bytesRead - pos));
            if (!end) {
                end = buffer.data() + bytesRead;
            }
            string line(buffer.data() + pos, end - (buffer.data() + pos));
            if (uniqueLines.find(line) == uniqueLines.end()) {
                uniqueLines.insert(line);
                ssize_t bytesWritten = lab2_write(outFile, (void *) line.c_str(), line.size(), outOffset);
                if (bytesWritten == -1) {
                    wcout << L"Произошла ошибка при записи строки в файл" << endl;
                    exit(1);
                }
                outOffset += bytesWritten;

                bytesWritten = lab2_write(outFile, (void *) "\n", 1, outOffset);
                if (bytesWritten == -1) {
                    wcout << L"Произошла ошибка при записи" << endl;
                    exit(1);
                }
                outOffset += bytesWritten;
            }
            pos = end - buffer.data() + 1;
        }
        globalOffset += bytesRead;
    }
    lab2_fsync(inFile);
    lab2_fsync(outFile);
    lab2_close(inFile);
    lab2_close(outFile);

    wcout << L"Дедупликация завершена" << endl;

    gettimeofday(&end, nullptr);
    long duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
    return duration;
}