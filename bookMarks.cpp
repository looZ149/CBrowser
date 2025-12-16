#include "bookMarks.h"
#include <fstream>
#include <filesystem>

static void SafeBookMark(const std::string &url) {
    char buffer[MAX_PATH];
    bool checkedDir = false;
#if defined(_WIN32)
#include <windows.h>
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    const std::filesystem::path exePath(buffer);
    const std::filesystem::path exeDir = exePath.parent_path();
    const std::string fileName = "Bookmarks.txt";

    do
    {
        //Don't we need to check for the full path here with exePath, instead of exeDir?
        if (std::filesystem::exists(exeDir) && std::filesystem::is_directory(exeDir))
        {
            //Only need to check if the dir exists, ofstream will create the file if it doesn't exist. Can we do the same for the dir?
            std::ofstream file(fileName);
            if (file.is_open())
            {
                file << url << std::endl;
                file.close();
                checkedDir = true;
            }
        }
        else
        {
            std::filesystem::create_directory(exeDir);
            //Jump back to start?
        }
    } while (!checkedDir);

#endif
#if defined(unix) || defined(__unix__) || defined(__unix)
    pathLen = std::filesystem::canonical("/proc/self/exe");
#endif
}





