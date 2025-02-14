#include <iostream>
#include <windows.h>
#include <queue>
#include <string>
#include <vector>
#include <sys/time.h>
#include <mutex>

using namespace std;
mutex cout_mutex;

bool findFile (const string& rootDir, const string& targetFileName) {
    setlocale(LC_ALL, "");
    queue<string> directories;
    directories.push(rootDir);

    while (!directories.empty()) {
        string currentDir = directories.front();
        directories.pop();

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA((currentDir + "\\*").c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            lock_guard<mutex> lock(cout_mutex);
            wcout << L"Ошибка доступа к каталогу" << endl;
            continue;
        }
        do {
            string fileName = findData.cFileName;

            if (fileName == "." || fileName == "..") {
                continue;
            }

            string fullPath = currentDir + "\\" + fileName;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                directories.push(fullPath);
            } else if (fileName == targetFileName) {
                FindClose(hFind);
                return true;
            }
        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
    }
    return false;
}

long searchName(const vector<string>& args, int repetitions) {
    setlocale(LC_ALL, "");
    if (args.size() != 2) {
        lock_guard<mutex> lock(cout_mutex);
        wcout << L"Неправильные входные данные" << endl;
        exit(1);
    }
    string rootDir = args[0];
    string targetFileName = args[1];

    struct timeval start, end;
    gettimeofday(&start, nullptr);

    for (int i = 0; i < repetitions; ++i) {
        bool fileFound = findFile(rootDir, targetFileName);

        lock_guard<mutex> lock(cout_mutex);
        if (fileFound) {
            wcout << L"Файл найден (Итерация " << i + 1 << ")" << endl;
        } else {
            wcout << L"Файл не найден (Итерация " << i + 1 << ")" << endl;
        }
    }
    gettimeofday(&end, nullptr);
    long duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

    return duration;
}