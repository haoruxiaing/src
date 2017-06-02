#ifndef __EMOJ_H__
#define  __EMOJ_H__

#include <string>
using namespace std;

class CEmojConvert
{
public:
    static void init();
    static void convert(const string& from, string& to);
};

#endif // __EMOJ_H__