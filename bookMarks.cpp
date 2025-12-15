#include "bookMarks.h"
#include <fstream>


// void CBrowserBookMarks::SaveBookMark(std::string &url)
// {
//
// }

static void SafeBookMark(std::string &url)
{
    std::string path;
#if defined(_WIN32)
    path = GetModuleFileNameA()
}



