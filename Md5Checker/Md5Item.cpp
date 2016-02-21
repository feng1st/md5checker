#include "stdafx.h"
#include "Md5Item.h"

int CMd5Item::m_iStateCategoryMap[0x20] =
{
    MIC_NA, MIC_UNKNOWN, 0, MIC_NEW,
    MIC_NA, MIC_LOADED, 0, MIC_FAILED,
    0, 0, 0, 0,
    0, 0, 0, MIC_PASSED,
    MIC_NA, MIC_UNKNOWN, 0, MIC_NEW,
    MIC_NA, MIC_LOADED, 0, MIC_FAILED,
    0, 0, 0, 0,
    0, 0, 0, MIC_PASSED
};

int CMd5Item::m_iStateIconMap[0x20] =
{
    1, 2, 0, 3,
    1, 2, 0, 4,
    0, 0, 0, 0,
    0, 0, 0, 5,
    6, 7, 0, 8,
    6, 7, 0, 9,
    0, 0, 0, 0,
    0, 0, 0, 10
};

CMd5Item *CMd5Item::Parse(LPCTSTR lpszLine, LPCTSTR lpszFolderFrom)
{
    TCHAR szSavedMD5[MAX_MD5];
    TCHAR szFilePath[MAX_PATH];

    // Trim left
    lpszLine = _tcsspnp(lpszLine, _T(" \t\r\n"));
    if (lpszLine == NULL)
        return NULL;

    // Copy MD5, otherwise command line
    if (!CFormatHelper::ReadMD5(lpszLine, szSavedMD5))
        return NULL;

    // Delimiter
    if (!CFormatHelper::ReadDelimiter(lpszLine))
        return NULL;

    // Path
    CPathHelper::Cleanup(lpszLine, szFilePath, _T("*?\"<>|\r\n"));
    CPathHelper::Resolve(szFilePath, szFilePath, lpszFolderFrom);
    if (*szFilePath == 0)
        return NULL;

    return new CMd5Item(szFilePath, szSavedMD5);
}

CMd5Item::CMd5Item()
{
    m_nState = MIS_NULL;
    *m_szFullPath = 0;
    m_lpszFileName = m_szFullPath;
    *m_szCurrentMD5 = 0;
    *m_szSavedMD5 = 0;
    m_nSize = 0;
    m_ftModified.dwLowDateTime = 0;
    m_ftModified.dwHighDateTime = 0;
}

CMd5Item::CMd5Item(LPCTSTR lpszFilePath)
{
    m_nState = MIS_NULL;
    *m_szFullPath = 0;
    m_lpszFileName = m_szFullPath;
    CPathHelper::GetFullPath(lpszFilePath, m_szFullPath, &m_lpszFileName);
    *m_szCurrentMD5 = 0;
    *m_szSavedMD5 = 0;
    Update();
}

CMd5Item::CMd5Item(LPCTSTR lpszFilePath, LPCTSTR lpszSavedMD5)
{
    m_nState = MIS_NULL;
    *m_szFullPath = 0;
    m_lpszFileName = m_szFullPath;
    CPathHelper::GetFullPath(lpszFilePath, m_szFullPath, &m_lpszFileName);
    *m_szCurrentMD5 = 0;
    SetSavedMD5(lpszSavedMD5);
}

CMd5Item::~CMd5Item()
{
}

void CMd5Item::Update()
{
    m_nState &= ~MIC_MASK;

    WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    if ((m_lpszFileName != m_szFullPath) && GetFileAttributesEx(m_szFullPath, GetFileExInfoStandard, &fileAttr))
    {
        m_nState |= MIS_AVAILABLE;
        m_nSize = (ULONGLONG)fileAttr.nFileSizeLow | ((ULONGLONG)fileAttr.nFileSizeHigh << 32);
        m_ftModified = fileAttr.ftLastWriteTime;
    }
    else
    {
        *m_szCurrentMD5 = 0;
        m_nSize = 0;
        m_ftModified.dwLowDateTime = 0;
        m_ftModified.dwHighDateTime = 0;
    }

    if (*m_szSavedMD5 != 0)
        m_nState |= MIS_OLD;

    if (*m_szCurrentMD5 != 0)
        m_nState |= MIS_NEW;

    if (((m_nState & (MIS_AVAILABLE | MIS_OLD | MIS_NEW)) == (MIS_AVAILABLE | MIS_OLD | MIS_NEW)) &&
        (_tcsicmp(m_szCurrentMD5, m_szSavedMD5) == 0))
        m_nState |= MIS_IDENTICAL;

    UpdateVersion();
}
