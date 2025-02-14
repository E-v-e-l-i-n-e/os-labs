#ifndef LAB1_DEDUP_H
#define LAB1_DEDUP_H
#include <string>
#include <vector>
#include <mutex>

using namespace std;
extern mutex mtx;

long dedup(const vector<string>& args, int repetitions);

#endif //LAB1_DEDUP_H
