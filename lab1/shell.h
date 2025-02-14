#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
using namespace std;

void commandCD(const std::wstring& line);
void runDedup(const wstring& line);
void runSearch(const wstring& line);
extern wstring current_directory;
bool runProgramAsUser(const wstring &programPathWithArguments);
wstring joinArgs(const vector<wstring> &args, const vector<size_t> &indices);
void dedupAndSearch(const wstring &line);
vector<wstring> split(const wstring &input);
#endif