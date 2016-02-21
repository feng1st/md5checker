#pragma once

#include "FormatHelper.h"
#include "PathHelper.h"

#define BYTES_MD5                   16
#define SIZE_MD5                    32
#define MAX_MD5                     33
#define MAX_SPACE_DELIMITERS        2
#define SIZE_NEWLINE                2
#define MAX_MD5_LINE                (SIZE_MD5 + MAX_SPACE_DELIMITERS + MAX_PATH + SIZE_NEWLINE)

// Md5 Item States
#define MIS_NULL            0x0
#define MIS_AVAILABLE       0x1
#define MIS_NEW             0x2
#define MIS_OLD             0x4
#define MIS_IDENTICAL       0x8
#define MIS_DISCARDED       0x10

// Md5 Item Category Mask
#define MIC_MASK            (MIS_AVAILABLE | MIS_NEW | MIS_OLD | MIS_IDENTICAL)

// Md5 Item Category
#define MIC_ALL             0
#define MIC_NA              1
#define MIC_UNKNOWN         2
#define MIC_LOADED          3
#define MIC_NEW             4
#define MIC_FAILED          5
#define MIC_PASSED          6

class CMd5Item
{
public:
    static CMd5Item *Parse(LPCTSTR lpszLine, LPCTSTR lpszFolderFrom);

    CMd5Item();
    CMd5Item(LPCTSTR lpszFilePath);
    CMd5Item(LPCTSTR lpszFilePath, LPCTSTR lpszSavedMD5);

    virtual ~CMd5Item();

public:

    virtual int GetVersion() const
    {
        return m_nVersion;
    }

    virtual int GetCategory() const
    {
        return m_iStateCategoryMap[m_nState];
    }

    virtual int GetIcon() const
    {
        return m_iStateIconMap[m_nState];
    }

    virtual LPCTSTR GetFullPath() const
    {
        return m_szFullPath;
    }

    virtual LPCTSTR GetFileName() const
    {
        return m_lpszFileName;
    }

    virtual LPCTSTR GetCurrentMD5() const
    {
        return m_szCurrentMD5;
    }

    virtual void SetCurrentMD5(BYTE bytes[])
    {
        CFormatHelper::ToHexString(bytes, BYTES_MD5, m_szCurrentMD5);
        Update();
    }

    virtual void SetCurrentMD5(LPCTSTR lpszCurrentMD5)
    {
        _tcsncpy_s(m_szCurrentMD5, MAX_MD5, lpszCurrentMD5, _TRUNCATE);
        Update();
    }

    virtual LPCTSTR GetSavedMD5() const
    {
        return m_szSavedMD5;
    }

    virtual void SetSavedMD5(LPCTSTR lpszSavedMD5)
    {
        _tcsncpy_s(m_szSavedMD5, MAX_MD5, lpszSavedMD5, _TRUNCATE);
        Update();
    }

    virtual ULONGLONG GetSize() const
    {
        return m_nSize;
    }

    virtual const FILETIME &GetModified() const
    {
        return m_ftModified;
    }

    virtual BOOL HasNew() const
    {
        return (m_nState & (MIS_AVAILABLE | MIS_NEW)) == (MIS_AVAILABLE | MIS_NEW);
    }

    virtual BOOL HasOld() const
    {
        return (m_nState & MIS_OLD) == MIS_OLD;
    }

    virtual BOOL IsNA() const
    {
        return (m_nState & MIS_AVAILABLE) == 0;
    }

    virtual BOOL HasNoMD5() const
    {
        return (m_nState & (MIS_NEW | MIS_OLD)) == 0;
    }

    virtual BOOL HasNoNew() const
    {
        return (m_nState & MIS_NEW) == 0;
    }

    virtual BOOL IsNew() const
    {
        return (m_nState & (MIS_AVAILABLE | MIS_NEW | MIS_OLD)) == (MIS_AVAILABLE | MIS_NEW);
    }

    virtual BOOL IsFailed() const
    {
        return (m_nState & (MIS_AVAILABLE | MIS_NEW | MIS_OLD | MIS_IDENTICAL)) == (MIS_AVAILABLE | MIS_NEW | MIS_OLD);
    }

    virtual BOOL IsDiscarded() const
    {
        return (m_nState & MIS_DISCARDED) == MIS_DISCARDED;
    }

    virtual LPCTSTR GetCopyString(LPTSTR lpszString) const
    {
        if (HasNew() && (_stprintf_s(lpszString, MAX_MD5_LINE, _T("%s  %s"), m_szCurrentMD5, m_lpszFileName) != -1))
            return lpszString;
        else
            return NULL;
    }

    virtual LPCTSTR GetSavingMD5() const
    {
        if (HasNew())
            return m_szCurrentMD5;
        else if (HasOld())
            return m_szSavedMD5;
        else
            return NULL;
    }

public:

    virtual void MergeExisted(const CMd5Item *pMd5Item)
    {
        if (pMd5Item != this)
        {
            if (HasOld() && pMd5Item->HasNew())
                SetCurrentMD5(pMd5Item->GetCurrentMD5());
            else if (!HasOld() && pMd5Item->HasOld())
                SetSavedMD5(pMd5Item->GetSavedMD5());
        }
    }

    virtual void MarkAsNA()
    {
        if (IsNA())
            return;
        m_nState &= ~(MIS_AVAILABLE | MIS_NEW);
        *m_szCurrentMD5 = 0;
        m_nSize = 0;
        m_ftModified.dwLowDateTime = 0;
        m_ftModified.dwHighDateTime = 0;
        UpdateVersion();
    }

    virtual void MarkAsDiscarded()
    {
        if (IsDiscarded())
            return;
        m_nState |= MIS_DISCARDED;
        UpdateVersion();
    }

    virtual void UnmarkAsDiscarded()
    {
        if (!IsDiscarded())
            return;
        m_nState &= ~MIS_DISCARDED;
        UpdateVersion();
    }

    virtual void UpdateMD5()
    {
        _tcsncpy_s(m_szSavedMD5, MAX_MD5, m_szCurrentMD5, _TRUNCATE);
        Update();
    }

protected:

    virtual void UpdateVersion()
    {
        m_nVersion = (m_nVersion + 1) % 0x40000000;
    }

    virtual void Update();

protected:
    static int m_iStateCategoryMap[0x20];
    static int m_iStateIconMap[0x20];

    int m_nVersion;
    int m_nState;

    TCHAR m_szFullPath[MAX_PATH];
    LPTSTR m_lpszFileName;
    TCHAR m_szCurrentMD5[MAX_MD5];
    TCHAR m_szSavedMD5[MAX_MD5];
    ULONGLONG m_nSize;
    FILETIME m_ftModified;
};
