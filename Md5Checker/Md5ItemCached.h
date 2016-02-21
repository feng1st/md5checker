#pragma once

#include "CommonDefine.h"
#include "Md5Item.h"

class CMd5ItemCached
{
public:

    CMd5ItemCached()
    {
    }

    virtual ~CMd5ItemCached()
    {
    }

public:

    virtual CMd5Item *GetMd5Item() const
    {
        return m_pMd5Item;
    }

    virtual LPCTSTR GetFolderPath() const
    {
        return m_szFolderPath;
    }

    virtual LPCTSTR GetSize() const
    {
        return m_szSize;
    }

    virtual LPCTSTR GetModified() const
    {
        return m_szModified;
    }

    virtual void SetMd5Item(CMd5Item *pMd5Item);

protected:
    int m_nVersion;

    CMd5Item *m_pMd5Item;

    TCHAR m_szFolderPath[MAX_PATH];
    TCHAR m_szSize[MAX_NUM_STR];
    TCHAR m_szModified[MAX_DATE_STR];
};
