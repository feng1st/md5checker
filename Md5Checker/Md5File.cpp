#include "stdafx.h"
#include "Md5File.h"

#include "Md5Checker.h"
#include "Options.h"
#include "PathHelper.h"

#define MODE_MASK           (CMd5File::ModeRead | CMd5File::ModeWrite | CMd5File::ModeAppend)
#define WRITE_PATH_MASK     (CMd5File::WritePathRelative | CMd5File::WritePathFileName | CMd5File::WritePathFull)

#define MAX_LINE            512

CMd5File::CMd5File()
{
    m_pFile = NULL;
    *m_szFolderPath = 0;
}

CMd5File::~CMd5File()
{
    Close();
}

BOOL CMd5File::Open(LPCTSTR lpszFilePath, int nFlag)
{
    if ((lpszFilePath == NULL) || (*lpszFilePath == 0))
        return FALSE;

    if ((m_pFile != NULL) && !Close())
        return FALSE;

    m_nFlag = nFlag;

    if ((m_nFlag & MODE_MASK) == ModeRead)
        m_pFile = _tfsopen(lpszFilePath, _T("rt"), _SH_DENYWR);
    else if ((m_nFlag & MODE_MASK) == ModeWrite)
        m_pFile = _tfsopen(lpszFilePath, _T("wb"), _SH_DENYRW);
    else if ((m_nFlag & MODE_MASK) == ModeAppend)
        m_pFile = _tfsopen(lpszFilePath, _T("ab"), _SH_DENYRW);
    if (m_pFile == NULL)
        return FALSE;

    if (((m_nFlag & MODE_MASK) == ModeRead) || ((m_nFlag & WRITE_PATH_MASK) == WritePathRelative))
    {
        DWORD n;
        LPTSTR p;
        if (((n = GetFullPathName(lpszFilePath, MAX_PATH, m_szFolderPath, &p)) > 0) && (n < MAX_PATH))
            PathRemoveFileSpec(m_szFolderPath);
    }

    COptions *pOptions = g_app.GetOptions();
    m_iItemDelimiter = pOptions->GetItemDelimiter();
    m_iLineFeed = pOptions->GetLineFeed();
    m_iPathDelimiter = pOptions->GetPathDelimiter();

    m_bFullLine = TRUE;

    return TRUE;
}

BOOL CMd5File::Read(CMd5Item *&rpMd5Item)
{
    if ((m_pFile == NULL) || (*m_szFolderPath == 0))
        return FALSE;

    TCHAR szLine[MAX_LINE];
    if (_fgetts(szLine, MAX_LINE, m_pFile) == NULL)
        return FALSE;

    rpMd5Item = NULL;
    if (m_bFullLine)
        rpMd5Item = CMd5Item::Parse(szLine, m_szFolderPath);

    if (rpMd5Item == NULL)
        m_bFullLine = (_tcslen(szLine) < (MAX_LINE - 1)) ||
            (szLine[MAX_LINE - 2] == _T('\r')) ||
            (szLine[MAX_LINE - 2] == _T('\n'));

    return TRUE;
}

BOOL CMd5File::Write(const CMd5Item *pMd5Item)
{
    if ((m_pFile == NULL) || (pMd5Item == NULL))
        return FALSE;

    LPCTSTR lpszSavingMD5 = pMd5Item->GetSavingMD5();
    if (lpszSavingMD5 == NULL)
        return FALSE;

    TCHAR szFilePath[MAX_PATH];
    TCHAR szLine[MAX_MD5_LINE];
    _stprintf_s(szLine, MAX_MD5_LINE, _T("%s%s%s%s"), lpszSavingMD5, GetItemDelimiter(), GetFilePath(pMd5Item, szFilePath), GetLineFeed());

#ifdef UNICODE
    size_t n;
    CHAR aszLine[MAX_MD5_LINE];
    wcstombs_s(&n, aszLine, MAX_MD5_LINE, szLine, MAX_MD5_LINE);
    fputs(aszLine, m_pFile);
#else
    _fputts(szLine, m_pFile);
#endif

    return TRUE;
}

BOOL CMd5File::Close()
{
    if (m_pFile == NULL)
        return FALSE;

    if (fclose(m_pFile) != 0)
        return FALSE;

    m_pFile = NULL;
    *m_szFolderPath = 0;

    return TRUE;
}

LPCTSTR CMd5File::GetItemDelimiter() const
{
    if (m_iItemDelimiter == MOE_ID_SPACES)
        return _T("  ");
    else if (m_iItemDelimiter == MOE_ID_VERTICAL_BAR)
        return _T("|");
    else
        return _T(" *");
}

LPCTSTR CMd5File::GetFilePath(const CMd5Item *pMd5Item, LPTSTR lpszFilePath) const
{
    if ((m_nFlag & WRITE_PATH_MASK) == WritePathFileName)
    {
        return pMd5Item->GetFileName();
    }
    else if (((m_nFlag & WRITE_PATH_MASK) == WritePathRelative) && (*m_szFolderPath != 0))
    {
        CPathHelper::RelatePathTo(pMd5Item->GetFullPath(), lpszFilePath, m_szFolderPath);
        if (m_iPathDelimiter == MOE_PD_SLASH)
            CPathHelper::GetSlashDelimitedPath(lpszFilePath, lpszFilePath);
        return lpszFilePath;
    }
    else if (m_iPathDelimiter == MOE_PD_SLASH)
    {
        CPathHelper::GetSlashDelimitedPath(pMd5Item->GetFullPath(), lpszFilePath);
        return lpszFilePath;
    }
    else
    {
        return pMd5Item->GetFullPath();
    }
}

LPCTSTR CMd5File::GetLineFeed() const
{
    if (m_iLineFeed == MOE_LF_LF)
        return _T("\n");
    else if (m_iLineFeed == MOE_LF_CR)
        return _T("\r");
    else
        return _T("\r\n");
}
