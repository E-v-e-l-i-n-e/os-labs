#include <windows.h>
#include <iostream>
#include "random_cache.h"

using namespace std;

CachePage cache[CACHE_SIZE];


void initCache() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache[i].fd = -1;
    }
}

int findFreePage() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].fd == -1) {
            return i;
        }
    }
    return -1;
}

CachePage* findPage(int fd, off_t offset) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].fd == fd && cache[i].offset == offset) {
            return &cache[i];
        }
    }
    return nullptr;
}

bool addPage(int fd, off_t offset, const char* data) {
    int index = findFreePage();
    if (index == -1) {
        index = evict();
        if (index == -1) {
            return false;
        }
    }

    cache[index].fd = fd;
    cache[index].offset = offset;
    memcpy(cache[index].data, data, PAGE_SIZE);
    cache[index].is_dirty = false;

    return true;
}

ssize_t readFromCache(int fd, void* buf, off_t offset, size_t bytesToRead) {
    CachePage *page = findPage(fd, offset);
    if (page) {
        memcpy(buf, page->data + (offset % PAGE_SIZE), bytesToRead);
        return (ssize_t)bytesToRead;
    } else {
        return -1;
    }
}

bool writeToCache(int fd, const char* data, size_t bytesToWrite, off_t pageOffset) {
    setlocale(LC_ALL, "");
    CachePage* page = findPage(fd, pageOffset);
    if (!page) {
        char dataForCache[PAGE_SIZE] = {0};
        if (!addPage(fd, pageOffset, dataForCache)) {
            wcout << L"Произошла ошибка при добавлении страницы в кэш" << endl;
            return false;
        }
        page = findPage(fd, pageOffset);
        if (!page) {
            wcout << L"Произошла ошибка. Страница не найдена после добавления в кэш" << endl;
            return false;
        }
    }
    memcpy(page->data + (pageOffset % PAGE_SIZE), data, bytesToWrite);
    page->is_dirty = true;
    return true;
}

int evict() {
    int index = rand() % CACHE_SIZE;
    if (cache[index].is_dirty) {
        DWORD bytesWritten;
        if (!WriteFile((HANDLE)cache[index].fd, cache[index].data, PAGE_SIZE, &bytesWritten, NULL)) {
            return -1;
        }
        cache[index].is_dirty = false;
    }
    return index;
}

int lab2_open(const char *path){
    setlocale(LC_ALL, "");

    DWORD attributes = GetFileAttributes(path);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        wcout << L"Не удалось прочитать файл" << endl;
        return -1;
    }

    HANDLE hFile = CreateFile(
            path,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_NO_BUFFERING,
            NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return -1;
    }
    return reinterpret_cast<intptr_t>(hFile);
}

int lab2_close(int fd) {
    setlocale(LC_ALL, "");
    HANDLE hFile = (HANDLE)fd;
    if (!CloseHandle(hFile)) {
        DWORD error = GetLastError();
        wcout << L"Произошла ошибка при закрытии файла" << endl << error << endl;
        return -1;
    }
    return 0;
}

int lab2_fsync(int fd) {
    setlocale(LC_ALL, "");
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].fd == fd && cache[i].is_dirty) {
            DWORD bytesWritten;
            if (!WriteFile((HANDLE)fd, cache[i].data, PAGE_SIZE, &bytesWritten, NULL)) {
                DWORD error = GetLastError();
                wcout << L"Произошла ошибка при синхронизации кэша" << error << endl;
                return -1;
            }
            cache[i].is_dirty = false;
        }
    }
    return 0;
}

ssize_t lab2_read(int fd, void* buf, size_t count, off_t offset) {
    setlocale(LC_ALL, "");
    size_t totalRead = 0;

    while (count > 0) {
        off_t pageOffset = offset % PAGE_SIZE;
        size_t bytesToRead = min(static_cast<size_t>(PAGE_SIZE - pageOffset), count);

        if (bytesToRead == 0) {
            break;
        }
        ssize_t bytesReadFromCache = readFromCache(fd, (char*)buf + totalRead, offset, bytesToRead);
        if (bytesReadFromCache > 0 ) {
            totalRead += bytesReadFromCache;
            offset += bytesReadFromCache;
            count -= bytesReadFromCache;
            continue;
        }
        char data[PAGE_SIZE];
        DWORD bytesRead;
        if (!ReadFile((HANDLE)fd, data, PAGE_SIZE, &bytesRead, NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_HANDLE_EOF) {
                wcout << L"Ошибка чтения с диска: " << error << endl;
                return -1;
            }
            break;
        }
        if (bytesRead == 0) {
            break;
        }

        if (!addPage(fd, offset - pageOffset, data)) {
            wcout << L"Ошибка добавления данных в кэш" << endl;
            return -1;
        }
        size_t actualBytesToRead = min(static_cast<size_t>(bytesRead), bytesToRead);
        memcpy((char*)buf + totalRead, data + pageOffset, actualBytesToRead);
        totalRead += actualBytesToRead;
        offset += actualBytesToRead;
        count -= actualBytesToRead;
    }
    return (ssize_t)totalRead;
}

ssize_t lab2_write(int fd, void* buf, size_t count, off_t offset) {
    setlocale(LC_ALL, "");
    size_t totalWritten = 0;

    while (count > 0) {
        off_t pageOffset = offset % PAGE_SIZE;
        size_t bytesToWrite = min(static_cast<size_t>(PAGE_SIZE - pageOffset), count);

        if (!writeToCache(fd, (const char*)buf + totalWritten, bytesToWrite, offset - pageOffset)) {
            wcout << L"Ошибка записи данных в кэш" << endl;
            return -1;
        }
        totalWritten += bytesToWrite;
        offset += bytesToWrite;
        count -= bytesToWrite;
    }
    return (ssize_t)totalWritten;
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    setlocale(LC_ALL, "");
    if (whence != SEEK_SET) {
        wcout << L"Поддерживаются только абсолютные координаты" << endl;
        return -1;
    }
    HANDLE hFile = (HANDLE)fd;

    LARGE_INTEGER liOffset;
    liOffset.QuadPart = offset;

    LARGE_INTEGER newPosition;
    if (!SetFilePointerEx(hFile, liOffset, &newPosition, FILE_BEGIN)) {
        DWORD error = GetLastError();
        wcout << L"Ошибка изменения позиции указателя" << endl << error << endl;
        return -1;
    }

    return newPosition.QuadPart;
}