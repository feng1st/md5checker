#pragma once

#include "Md5ItemCached.h"
#include "Md5ItemList.h"

#define MAX_MD5_CACHE               256

#define CACHED_ONLY_CATEGORY        -1

class CMd5ItemCache
{
public:
    CMd5ItemCache(CMd5ItemList *pMd5ItemList);
    virtual ~CMd5ItemCache();

public:

    virtual CMd5ItemCached *GetMd5ItemCached(int i, int nCount, int iCategory)
    {
        if (Cache(i, nCount, iCategory))
            return &m_md5ItemCached[(m_iInternal + (i - m_iExternal)) % MAX_MD5_CACHE];
        else
            return NULL;
    }

    virtual CMd5Item *GetMd5Item(int i, int nCount, int iCategory) const;
    virtual int GetIndex(const CMd5Item *pMd5Item, int iCategory) const; // iCategory can be CACHED_ONLY_CATEGORY

public:

    virtual void Cache(int iMin, int iMax, int nCount, int iCategory)
    {
        if ((iMin > iMax) || (iMin < 0) || (iMax >= nCount))
            return;

        if (iMin < m_iExternal)
            Cache(iMin, nCount, iCategory);
        if (iMax >= (m_iExternal + m_nSize))
            Cache(iMax, nCount, iCategory);
    }

    virtual void Clear()
    {
        m_iInternal = 0;
        m_iExternal = 0;

        m_nSize = 0;
    }

    virtual BOOL Cache(int i, int nCount, int iCategory);
    // Do NOT recache before actual list item is deleted!
    virtual void Delete(int i, int nCount, int iCategory, BOOL bRecache = TRUE);
    // Do NOT recache before actual list item is inserted!
    virtual void Insert(int i, int nCount, int iCategory, BOOL bRecache = TRUE);

protected:
    CMd5ItemList *m_pMd5ItemList;

    int m_iInternal;
    int m_iExternal;

    CMd5ItemCached m_md5ItemCached[MAX_MD5_CACHE];

    int m_nSize;
};
