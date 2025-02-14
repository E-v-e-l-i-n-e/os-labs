#include <iostream>
#include "shell.h"


using namespace std;


//g++ -O0 -o shell main.cpp shell.cpp

// dedup.exe random_numbers256.txt dedup_numbers.txt 1 1
// searchName.exe C:\\Users\\79124\\Documents\\ElinaComputer Project.gsproj 2 1
// dedupAndSearch.exe C:\\Users\\79124\\Documents\\ElinaComputer Project.gsproj random_numbers256.txt dedup_numbers.txt 2 2
// notepad.exe dedup_numbers.txt

/*
wpr -start CPU -filemode
wpr -stop search50.etl
 */
int main() {
    setlocale(LC_ALL, "");

    while (true) {
        wcout << current_directory << L">";
        wstring line;
        getline(wcin, line);

        if (line.substr(0, 3) == L"cd ") {
            commandCD(line);

        } else if (line == L"exit") {
            break;

        } else {
            bool isProcess = runProgramAsUser(line);
            if (!isProcess)
                wcout << L"Неизвестная команда" << endl;
        }
    }
    wcout << L"Пака, пака!" << endl;
    return 0;
}
