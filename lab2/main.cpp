#include <iostream>
#include "random_cache.h"
#include "dedup.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "");;
    initCache();
    wcout << L"Кэш инициализирован" << endl;

    long time_without_cache = dedup({"random_numbers256.txt", "dedup_numbers.txt"});
    wcout << L"Время выполнения без кэша: " << time_without_cache << L" мс" << endl;

    long time_with_cache = dedup_with_cache({"random_numbers256.txt", "dedup_numbers.txt"});
    wcout << L"Время выполнения с кэшем: " << time_with_cache << L" мс" << endl;

    return 0;
}
