#include "stdafx.h"
#include "FormatHelper.h"

#include "CommonDefine.h"
#include "Md5Item.h"

LPCTSTR CFormatHelper::ToHexString(BYTE bytes[], int n, LPTSTR lpszString)
{
    int iChar = 0;
    for (int iByte = 0; iByte < n; iByte++)
    {
        lpszString[iChar++] = HEX_TABLE[bytes[iByte] >> 4];
        lpszString[iChar++] = HEX_TABLE[bytes[iByte] & 0x0F];
    }
    lpszString[iChar] = 0;

    return lpszString;
}

BOOL CFormatHelper::IsValidMD5(LPCTSTR lpszMD5)
{
    if ((lpszMD5 == NULL) || (*lpszMD5 == 0))
        return FALSE;

    int i = 0;
    while (_istxdigit(lpszMD5[i]) && (i < SIZE_MD5))
        i++;

    return (i == SIZE_MD5) && (lpszMD5[SIZE_MD5] == 0);
}

BOOL CFormatHelper::ReadMD5(LPCTSTR &lpszSrc, LPTSTR lpszMD5)
{
    if ((lpszSrc == NULL) || (*lpszSrc == 0) || (lpszMD5 == NULL))
        return FALSE;

    int n = 0;
    while (_istxdigit(lpszSrc[n]) && (n < SIZE_MD5))
        n++;

    if ((n == 0) || _istxdigit(lpszSrc[n])) // no hex char or too many hex chars
        return FALSE;

    lpszMD5[SIZE_MD5] = 0;
    if (n < SIZE_MD5) // leading zeros
        _tcsnset_s(lpszMD5, MAX_MD5, _T('0'), SIZE_MD5 - n);
    _tcsncpy_s(lpszMD5 + SIZE_MD5 - n, n + 1, lpszSrc, n);

    lpszSrc += n;

    return TRUE;
}

BOOL CFormatHelper::ReadDelimiter(LPCTSTR &lpszSrc)
{
    if ((lpszSrc == NULL) || (*lpszSrc == 0))
        return FALSE;

    int n, nNonSpace = -1;
    for (n = 0; (lpszSrc[n] != 0) && (_tcschr(_T(" \t*?\"<>|\r\n"), lpszSrc[n]) != NULL); n++)
    {
        if (lpszSrc[n] != _T(' ')) // last non-space delimiter
            nNonSpace = n;
    }

    if ((n == 0) || (lpszSrc[n] == 0)) // no delimiter or no following file path
        return FALSE;
    else if (nNonSpace != -1) // delimiters + non-space delimiter + spaces in file path
        lpszSrc += nNonSpace + 1;
    else // space delimiters (max two chars) + spaces in file path
        lpszSrc += min(n, MAX_SPACE_DELIMITERS);

    return TRUE;
}
