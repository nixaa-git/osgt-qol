#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define EXPORT __declspec(dllexport)

using DirectInput8Create_t = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPVOID);

// Setup function called in InitializeIfNeeded.
void setup();

// Initializes the library if it hasn't been initialized yet.
void InitializeIfNeeded()
{
    static long initialized = 0;
    if (InterlockedCompareExchange(&initialized, 1, 0) == 0)
        CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(setup), NULL, 0, NULL);
}

// Initializes the library if needed and calls original DirectInput8Create.
EXPORT HRESULT WINAPI DirectInput8Create(HINSTANCE instance, DWORD version, REFIID id, LPVOID* out,
                                         LPVOID unk)
{
    InitializeIfNeeded();
    // GetSystemDirectoryW(nullptr, 0) will returns the size of the buffer needed to store the path
    // without the null terminator.
    uint32_t sysDirPathSize = GetSystemDirectoryW(nullptr, 0);
    std::wstring path(sysDirPathSize - 1, L'\0');
    GetSystemDirectoryW(&path[0], sysDirPathSize);
    path += L"\\dinput8.dll";
    // Assume dinput8 is on the system, the game depends on it anyway.
    HMODULE dinput8 = LoadLibraryW(path.c_str());
    auto fn = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
    return fn != nullptr ? fn(instance, version, id, out, unk) : E_FAIL;
}