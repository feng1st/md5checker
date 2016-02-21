#pragma once

#include "Md5Item.h"

class CMd5File
{
public:
    static const int ModeRead           = 0x0;
    static const int ModeWrite          = 0x1;
    static const int ModeAppend         = 0x2;

    static const int WritePathRelative  = 0x0;
    static const int WritePathFileName  = 0x4;
    static const int WritePathFull      = 0x8;

public:
    CMd5File();
    virtual ~CMd5File();

public:
    virtual BOOL Open(LPCTSTR lpszFilePath, int nFlag);
    virtual BOOL Read(CMd5Item *&rpMd5Item);
    virtual BOOL Write(const CMd5Item *pMd5Item);
    virtual BOOL Close();

protected:
    virtual LPCTSTR GetItemDelimiter() const;
    virtual LPCTSTR GetFilePath(const CMd5Item *pMd5Item, LPTSTR lpszFilePath) const;
    virtual LPCTSTR GetLineFeed() const;

protected:
    FILE *m_pFile;
    int m_nFlag;
    int m_iItemDelimiter;
    int m_iLineFeed;
    int m_iPathDelimiter;
    TCHAR m_szFolderPath[MAX_PATH];

    BOOL m_bFullLine;
};
