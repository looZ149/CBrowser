#ifndef CBROWSER_BOOKMARKS_H
#define CBROWSER_BOOKMARKS_H
#include <string>
#if defined(_WIN32)
#include <Windows.h>
#endif

class CBrowserBookMarks
{
public:
    static void SafeBookMark(std::string &url);
};



#endif //CBROWSER_BOOKMARKS_H