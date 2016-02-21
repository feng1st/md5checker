#pragma once

#include <functional>
#include <map>

#include "Md5ItemList.h"

using namespace std;

struct _tmd5filenameless : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_tcsicmp(_Left->GetFileName(), _Right->GetFileName()) < 0);
    }
};

struct _tmd5filenamegreater : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_tcsicmp(_Left->GetFileName(), _Right->GetFileName()) > 0);
    }
};

struct _tmd5folderpathless : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        int nFolderPathLeft = (int)(_Left->GetFileName() - _Left->GetFullPath());
        int nFolderPathRight = (int)(_Right->GetFileName() - _Right->GetFullPath());
        if (nFolderPathRight == 0)
            return false;
        else if (nFolderPathLeft == 0)
            return true;

        int n = _tcsnicmp(_Left->GetFullPath(), _Right->GetFullPath(), min(nFolderPathLeft, nFolderPathRight) - 1);
        return ((n < 0) || ((n == 0) && (nFolderPathLeft < nFolderPathRight)));
    }
};

struct _tmd5folderpathgreater : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        int nFolderPathLeft = (int)(_Left->GetFileName() - _Left->GetFullPath());
        int nFolderPathRight = (int)(_Right->GetFileName() - _Right->GetFullPath());
        if (nFolderPathLeft == 0)
            return false;
        else if (nFolderPathRight == 0)
            return true;

        int n = _tcsnicmp(_Left->GetFullPath(), _Right->GetFullPath(), min(nFolderPathLeft, nFolderPathRight) - 1);
        return ((n > 0) || ((n == 0) && (nFolderPathLeft > nFolderPathRight)));
    }
};

struct _tmd5currentmd5less : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_tcsicmp(_Left->GetCurrentMD5(), _Right->GetCurrentMD5()) < 0);
    }
};

struct _tmd5currentmd5greater : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_tcsicmp(_Left->GetCurrentMD5(), _Right->GetCurrentMD5()) > 0);
    }
};

struct _tmd5savedmd5less : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_tcsicmp(_Left->GetSavedMD5(), _Right->GetSavedMD5()) < 0);
    }
};

struct _tmd5savedmd5greater : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_tcsicmp(_Left->GetSavedMD5(), _Right->GetSavedMD5()) > 0);
    }
};

struct _tmd5sizeless : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_Left->GetSize() < _Right->GetSize());
    }
};

struct _tmd5sizegreater : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return (_Left->GetSize() > _Right->GetSize());
    }
};

struct _tmd5modifiedless : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return ((_Left->GetModified().dwHighDateTime < _Right->GetModified().dwHighDateTime) ||
            ((_Left->GetModified().dwHighDateTime == _Right->GetModified().dwHighDateTime) &&
            (_Left->GetModified().dwLowDateTime < _Right->GetModified().dwLowDateTime)));
    }
};

struct _tmd5modifiedgreater : public binary_function<CMd5Item *, CMd5Item *, bool>
{
    bool operator()(const CMd5Item *_Left, const CMd5Item *_Right) const
    {
        return ((_Left->GetModified().dwHighDateTime > _Right->GetModified().dwHighDateTime) ||
            ((_Left->GetModified().dwHighDateTime == _Right->GetModified().dwHighDateTime) &&
            (_Left->GetModified().dwLowDateTime > _Right->GetModified().dwLowDateTime)));
    }
};

class CMd5ItemSortHelper
{
private:
    CMd5ItemSortHelper() { }

public:

    template<class _Pr>
    static void Sort(CMd5ItemList *pMd5ItemList, int iCategory)
    {
        multiset<CMd5Item *, _Pr> setSort;

        CMd5Item *pMd5Item;
        CMd5ItemList::Iter iter = pMd5ItemList->GetFirstIter();
        CMd5ItemList::Iter iterCurr = iter;
        while ((pMd5Item = pMd5ItemList->GetNext(iter)) != NULL)
        {
            if ((iCategory == 0) || (pMd5Item->GetCategory() == iCategory))
            {
                setSort.insert(pMd5Item);
                pMd5ItemList->MoveOut(iterCurr);
            }

            iterCurr = iter;
        }

        for (multiset<CMd5Item *, _Pr>::reverse_iterator set_rIter = setSort.rbegin(); set_rIter != setSort.rend(); set_rIter++)
            pMd5ItemList->MoveIn(*set_rIter);

        setSort.clear();
    }
};
