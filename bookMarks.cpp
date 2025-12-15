#include "bookMarks.h"
#include <fstream>
#include <filesystem>


// void CBrowserBookMarks::SaveBookMark(std::string &url)
// {
//
// }

static void SafeBookMark(std::string &url) {
    std::string path;
#if defined(_WIN32)
    path = GetModuleFileNameA(NULL, path, MAX_PATH)
    if (DirectoryExists)
#endif
#if defined(unix) || defined(__unix__) || defined(__unix)
    path = std::filesystem::canonical("/proc/self/exe");
#endif
}





