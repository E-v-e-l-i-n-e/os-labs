#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef LAB2_RANDOM_CASHE_H
#define LAB2_RANDOM_CASHE_H
#ifdef RANDOM_CACHE_EXPORTS

#define RANDOM_CACHE_API __declspec(dllexport)
#else
#define RANDOM_CACHE_API __declspec(dllimport)
#endif

const int PAGE_SIZE = 4096;
const int CACHE_SIZE = 100;
struct CachePage {
    int fd;
    off_t offset;
    char data[PAGE_SIZE];
    bool is_dirty;
};

//void initCache();
//int findFreePage();
//CachePage* findPage(int fd, off_t offset);
//bool addPage(int fd, off_t offset, const char* data);
//ssize_t readFromCache(int fd, void* buf, off_t offset, size_t bytesToRead);
//bool writeToCache(int fd, const char* data, size_t bytesToWrite, off_t pageOffset);
//
//
//
//int lab2_open(const char *path);
//int lab2_close(int fd);
//ssize_t lab2_read(int fd, void* buf, size_t count, off_t offset);
//ssize_t lab2_write(int fd, void* buf, size_t count, off_t offset) ;
//off_t lab2_lseek(int fd, off_t offset, int whence);
//int lab2_fsync(int fd);


#define CACHE_SIZE 100 // Количество страниц в кэше
#define PAGE_SIZE 4096 // Размер страницы (4 KB)


//int evict();

//CachePage cache[CACHE_SIZE];

// Экспорт функций через RANDOM_CACHE_API
extern "C" { // Для обеспечения C-совместимости имен
RANDOM_CACHE_API void initCache();
RANDOM_CACHE_API int findFreePage();
RANDOM_CACHE_API CachePage* findPage(int fd, off_t offset);
RANDOM_CACHE_API bool addPage(int fd, off_t offset, const char* data);
RANDOM_CACHE_API ssize_t readFromCache(int fd, void* buf, off_t offset, size_t bytesToRead);
RANDOM_CACHE_API bool writeToCache(int fd, const char* data, size_t bytesToWrite, off_t pageOffset);

RANDOM_CACHE_API int lab2_open(const char* path);
RANDOM_CACHE_API int lab2_close(int fd);
RANDOM_CACHE_API ssize_t lab2_read(int fd, void* buf, size_t count, off_t offset);
RANDOM_CACHE_API ssize_t lab2_write(int fd, void* buf, size_t count, off_t offset);
RANDOM_CACHE_API off_t lab2_lseek(int fd, off_t offset, int whence);
RANDOM_CACHE_API int lab2_fsync(int fd);

RANDOM_CACHE_API int evict();
}

#endif //LAB2_RANDOM_CASHE_H
