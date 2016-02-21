#pragma once

#include <set>

#include "Md5Item.h"

using namespace std;

class CCheckThread
{
public:
    static UINT WINAPI ThreadProc(LPVOID lpParameter);

public:
    CCheckThread();
    virtual ~CCheckThread();

public:

    virtual BOOL IsChecking() const
    {
        return m_bChecking;
    }

    virtual LPCTSTR GetCurrentFilePath() const
    {
        return m_szCurrentFilePath;
    }

    virtual int GetFileCompletedPercent() const
    {
        if (m_nFileSize.QuadPart > 0)
            return (int)(m_nRead.QuadPart * 100.0 / m_nFileSize.QuadPart);
        else
            return 0;
    }

public:
    virtual void Start();
    virtual void Stop();
    virtual void CheckStop();
    virtual void CheckDeleted();

protected:

    virtual BOOL IsMd5ItemUnknown(CMd5Item *pMd5Item, BOOL bNewOnly)
    {
        return (bNewOnly ? pMd5Item->HasNoMD5() : pMd5Item->HasNoNew()) &&
            !(pMd5Item->IsNA() && (m_setNAMd5Item.find(pMd5Item) != m_setNAMd5Item.end()));
    }

    virtual BOOL GetNextFilePath();

protected:
    HANDLE m_hThread;

    BOOL m_bChecking;
    BOOL m_bSkip;

    TCHAR m_szCurrentFilePath[MAX_PATH];
    LARGE_INTEGER m_nFileSize;
    LARGE_INTEGER m_nRead;

    set<CMd5Item *> m_setNAMd5Item;
};
