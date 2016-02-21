#include "stdafx.h"
#include "PathHelper.h"

LPTSTR CPathHelper::Cleanup(LPCTSTR lpszPath, LPTSTR lpszClean, LPCTSTR lpszInvalid)
{
    if (lpszClean != lpszPath)
        _tcsncpy_s(lpszClean, MAX_PATH, lpszPath, _TRUNCATE);

    if (lpszInvalid == NULL)
        lpszInvalid = _T("\\/:*?\"<>|\r\n");

    LPTSTR p = _tcspbrk(lpszClean, lpszInvalid);
    if (p != NULL)
        *p = 0;

    return lpszClean;
}

LPTSTR CPathHelper::Resolve(LPCTSTR lpszPath, LPTSTR lpszAbsPath, LPCTSTR lpszFolderFrom)
{
    TCHAR szPath[MAX_PATH];

    if (PathIsRelative(lpszPath))
    {
        _tcsncpy_s(szPath, MAX_PATH, lpszFolderFrom, _TRUNCATE);
        PathAppend(szPath, lpszPath);
        _tcsncpy_s(lpszAbsPath, MAX_PATH, szPath, _TRUNCATE);
    }
    else if ((lpszPath[0] == _T('\\')) && (lpszPath[1] != _T('\\')))
    {
        _tcsncpy_s(szPath, MAX_PATH, lpszFolderFrom, _TRUNCATE);
        PathStripToRoot(szPath);
        PathAppend(szPath, lpszPath);
        _tcsncpy_s(lpszAbsPath, MAX_PATH, szPath, _TRUNCATE);
    }
    else if (lpszAbsPath != lpszPath)
    {
        _tcsncpy_s(lpszAbsPath, MAX_PATH, lpszPath, _TRUNCATE);
    }

    return lpszAbsPath;
}

LPTSTR CPathHelper::RelatePathTo(LPCTSTR lpszPath, LPTSTR lpszRelPath, LPCTSTR lpszFolderFrom)
{
    TCHAR szPath[MAX_PATH];

    if (PathIsSameRoot(lpszPath, lpszFolderFrom))
    {
        PathRelativePathTo(szPath, lpszFolderFrom, FILE_ATTRIBUTE_DIRECTORY, lpszPath, FILE_ATTRIBUTE_NORMAL);
        if ((szPath[0] == _T('.')) && (szPath[1] == _T('\\')))
        {
            _tcsncpy_s(lpszRelPath, MAX_PATH, &szPath[2], _TRUNCATE);
        }
        else if (lpszRelPath != lpszPath)
        {
            _tcsncpy_s(lpszRelPath, MAX_PATH, lpszPath, _TRUNCATE);
        }
    }
    else if (lpszRelPath != lpszPath)
    {
        _tcsncpy_s(lpszRelPath, MAX_PATH, lpszPath, _TRUNCATE);
    }

    return lpszRelPath;
}

LPTSTR CPathHelper::GetFullPath(LPCTSTR lpszPath, LPTSTR lpszFullPath, LPTSTR *lpszFilePart)
{
    TCHAR szPath[MAX_PATH];

    _tcsncpy_s(szPath, MAX_PATH, lpszPath, _TRUNCATE);

    if (PathFileExists(szPath))
        GetLongPathName(szPath, szPath, MAX_PATH);

    LPTSTR p = NULL;
    GetFullPathName(szPath, MAX_PATH, lpszFullPath, &p);
    if ((lpszFilePart != NULL) && (p != NULL))
        *lpszFilePart = p;

    return lpszFullPath;
}

LPTSTR CPathHelper::GetSlashDelimitedPath(LPCTSTR lpszPath, LPTSTR lpszSlashDelimited)
{
    if (lpszSlashDelimited != lpszPath)
        _tcsncpy_s(lpszSlashDelimited, MAX_PATH, lpszPath, _TRUNCATE);

    LPTSTR p = lpszSlashDelimited;
    while ((p = _tcschr(p, _T('\\'))) != NULL)
        *p++ = _T('/');

    return lpszSlashDelimited;
}
