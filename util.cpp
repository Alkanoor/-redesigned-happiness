#include "util.hpp"


std::string file(const std::string& in)
{
    std::string res;
    int i = (int)in.size()-1;
    while(i>=0&&in[i]!='/')
    {
        res = in[i]+res;
        i--;
    }
    return res;
}
