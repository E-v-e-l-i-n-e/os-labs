#include <iostream>
#include <windows.h>
#include <vector>
#include <sstream>
#include <string>
#include <sys/time.h>

using namespace std;

wstring current_directory = L".";

vector<wstring> split(const wstring &input) {
    vector<wstring> result;
    wstringstream stream(input);
    wstring token;
    while (stream >> token) {
        result.push_back(token);
    }
    return result;
}

void commandCD(const wstring &line) {
    const wchar_t *path = line.substr(3).c_str();
    wcout << path << endl;
    if (SetCurrentDirectoryW(path)) {
        wchar_t buffer[MAX_PATH];
        if (GetCurrentDirectoryW(MAX_PATH, buffer)) {
            current_directory = buffer;
        }
    } else {
        wcout << L"Неправильный путь" << endl;
    }
}

bool runProgramAsUser(const wstring &programPathWithArguments) {
    const wstring path = programPathWithArguments;

    HANDLE hToken = nullptr;
    HANDLE hDupToken = nullptr;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
        cerr << "OpenProcessToken failed: " << GetLastError() << endl;
        return false;
    }

    if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenPrimary, &hDupToken)) {
        cerr << "DuplicateTokenEx failed: " << GetLastError() << endl;
        CloseHandle(hToken);
        return false;
    }

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    struct timeval start, end;
    gettimeofday(&start, nullptr);

    if (!CreateProcessAsUserW(
            hDupToken,
            nullptr,
            const_cast<wchar_t*>(path.c_str()),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi
    )) {
//        cerr << "CreateProcessAsUser failed: " << GetLastError() << endl;
        CloseHandle(hDupToken);
        CloseHandle(hToken);
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    gettimeofday(&end, nullptr);
    long duration = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hDupToken);
    CloseHandle(hToken);

    wcout << L"Process was active for " << duration << L" мс" << endl;
    return true;
}
