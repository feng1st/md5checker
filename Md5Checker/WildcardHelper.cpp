#include "stdafx.h"
#include "WildcardHelper.h"

BOOL CWildcardHelper::Match(LPCTSTR lpszFileName, LPCTSTR lpszWildcards, BOOL bEmptyMatchAll)
{
    if ((lpszFileName == NULL) || (*lpszFileName == 0))
        return FALSE;

    if ((lpszWildcards == NULL) || (*lpszWildcards == 0))
        return bEmptyMatchAll;

    LPCTSTR p = lpszWildcards;
    while ((p != NULL) && ((p = _tcsspnp(p, _T(" ;"))) != NULL)) // skip delimiter
    {
        if (DoMatch(lpszFileName, p))
            return TRUE;

        p = _tcspbrk(p, _T(" ;")); // next delimiter. terminating null if not found
    }
    return FALSE;
}

BOOL CWildcardHelper::DoMatch(LPCTSTR lpszFileName, LPCTSTR lpszWildcard, int nWordLength)
{
    int nFileName;

    switch (*lpszWildcard)
    {
    case 0:
    case _T(' '):
    case _T(';'):
        return *lpszFileName == 0;

    case _T('?'):
        if (*lpszFileName == 0)
            return FALSE;

        if ((lpszWildcard[1] == 0) || (lpszWildcard[1] == _T(' ')) || (lpszWildcard[1] == _T(';'))) // tailing question mark
            return (lpszFileName[1] == 0);

        return DoMatch(lpszFileName + 1, lpszWildcard + 1);

    case _T('*'):
        if ((lpszWildcard[1] == 0) || (lpszWildcard[1] == _T(' ')) || (lpszWildcard[1] == _T(';'))) // tailing asterisk
            return TRUE;

        nFileName = (int)_tcslen(lpszFileName);
        nWordLength = (int)_tcscspn(lpszWildcard + 1, _T(" ;*?"));
        for (int i = 0; i <= nFileName - nWordLength; i++)
        {
            if (DoMatch(lpszFileName + i, lpszWildcard + 1, nWordLength))
                return TRUE;
        }
        return FALSE;

    default:
        if (*lpszFileName == 0)
            return FALSE;

        if (nWordLength == 0)
            nWordLength = (int)_tcscspn(lpszWildcard, _T(" ;*?")); // nWordLength > 0

        if (_tcsnicmp(lpszFileName, lpszWildcard, nWordLength) != 0)
            return FALSE;

        if ((lpszWildcard[nWordLength] == 0) || (lpszWildcard[nWordLength] == _T(' ')) || (lpszWildcard[nWordLength] == _T(';')))
            return lpszFileName[nWordLength] == 0;

        return DoMatch(lpszFileName + nWordLength, lpszWildcard + nWordLength);
    }
}
