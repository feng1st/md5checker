#pragma once

#include <functional>

using namespace std;

template<class _Ty>
struct _tcsiless : public binary_function<_Ty, _Ty, bool>
{
    bool operator()(const _Ty &_Left, const _Ty &_Right) const
    {
        return (_tcsicmp(_Left, _Right) < 0);
    }
};
