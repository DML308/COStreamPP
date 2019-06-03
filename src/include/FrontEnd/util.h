#ifndef _UTIL_H_
#define _UTIL_H_
#include "defines.h"

/* 由于 C++编译器对模板函数的限制, 模板函数的声明和实现必须都放在.h 文件中, 若把实现放在.cpp 文件中则会遇到链接错误 */


template <typename T>
/**
* (工具函数)把一个 vector 转化为 string, 要求 vector 的内部是简单类型(int,double,string)
* 返回值例子: [1,2,3,4,5]
*/
string vectorToString(vector<T> v)
{
    int k = 0;
    string str = "[";
    for (auto i : v)
    {
        stringstream sstr;
        sstr << i;
        str += (k++ > 0 ? "," : "") + sstr.str();
    }
    return str + "]";
}

#endif