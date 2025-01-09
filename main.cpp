#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include "BigInt.h"

typedef BigInt<1024> UINT;

void CacheContentError()
{
    std::cerr << "Unexpected contents of cache file\n";
    exit(1);
}

const char* GetCacheFileName()
{
    static std::string name = std::string("fibonacciNumbers.") + std::to_string(sizeof(UINT)) + std::string("bin");
    return name.c_str();
}

int GetCachedCount(int cacheFile)
{
    struct stat fileStat;
    if (fstat(cacheFile, &fileStat) == 0) 
        return fileStat.st_size / sizeof(UINT);

    return 0;
}

void GetInitialNumbers(int cacheFile, int n, UINT* f1, UINT* f2)
{
    *f1 = 0;
    *f2 = 1;

    if (cacheFile == -1)
        return;
    
    int cachedCount = GetCachedCount(cacheFile);

    if(cachedCount <= 2)
        return;
    
    lseek(cacheFile, (cachedCount-2) * sizeof(UINT), SEEK_SET);
    if(!read(cacheFile, f1, sizeof(UINT))) CacheContentError();
    if(!read(cacheFile, f2, sizeof(UINT))) CacheContentError();
}

UINT Fib(int n, bool useCache)
{
    int cacheFile = -1;
    if(useCache)
        cacheFile = open(GetCacheFileName(), O_RDWR | O_CREAT | O_APPEND,0644);

    int cachedCount = GetCachedCount(cacheFile);
    if(n < cachedCount - 1)
    {
        lseek(cacheFile, (n) * sizeof(UINT), SEEK_SET);
        UINT num;
        if(!read(cacheFile, &num, sizeof(UINT)))CacheContentError();
        close(cacheFile);
        return num;
    }

    UINT f1, f2;
    GetInitialNumbers(cacheFile, n, &f1, &f2);

    for(int i = cachedCount; i <= n; i++)
    {
        if(i == 0)
        {
            if(!write(cacheFile, &f1, sizeof(UINT)))CacheContentError();
            continue;
        }
        else if(i == 1)
        {
            if(!write(cacheFile, &f2, sizeof(UINT)))CacheContentError();
            continue;
        }

        std::swap(f1,f2);
        f2 = f1+f2;
        if(!write(cacheFile, &f2, sizeof(UINT)))CacheContentError();
    }
    
    close(cacheFile);
    return f2;
}

int main(int argC, char** argV)
{
    // Czesc zajmujaca sie argumentami.
    if(argC < 2)
    {
        std::cerr << "Program wymaga conajmniej jednego argumentu, podano " << argC-1 << '\n';
        return 1;
    }

    int n = 9;
    try{ n = std::stoi(std::string(argV[1])); }
    catch(...)
    {
        std::cerr << "Pierwszy argument musi byc liczba\n";
        return 1;
    }
    std::vector<std::string> optionalArgs;
    for(int i = 2; i < argC; i++)
        optionalArgs.push_back(std::string(argV[i]));

    bool useCache = std::find(optionalArgs.begin(),optionalArgs.end(), "-noCache") == optionalArgs.end();
    bool showCache = std::find(optionalArgs.begin(),optionalArgs.end(), "-showCache") != optionalArgs.end();

    // Wywoywanie funkcji Fib i pomiar czasu wykonywania.
    auto start = std::chrono::high_resolution_clock::now();
    UINT fib = Fib(n, useCache);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(end - start);

    // Wypisywanie wynikow.
    std::cout << fib << '\n' << "Calculations took: " << duration.count() << "ms." << '\n';

    if(showCache)
    {
        int cacheFile = open(GetCacheFileName(), O_RDWR | O_CREAT | O_APPEND,0644);
        std::cout << "CacheContents: \n";
        int cachedCount = GetCachedCount(cacheFile);
        for(int i =0; i < cachedCount; i++)
        {
            UINT num;
            if(!read(cacheFile, &num, sizeof(UINT)))CacheContentError();
            std::cout << i << ": " << num << '\n';
        }
        close(cacheFile);
    }

    return 0;
}
