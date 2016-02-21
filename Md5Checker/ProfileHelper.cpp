#include "stdafx.h"
#include "ProfileHelper.h"

#include "CommonDefine.h"

BOOL CProfileHelper::WritePrivateProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue, LPCTSTR lpszProfileName)
{
    TCHAR szInt[MAX_INT_STR];
    _stprintf_s(szInt, MAX_INT_STR, _T("%d"), nValue);
    return WritePrivateProfileString(lpszSection, lpszEntry, szInt, lpszProfileName);
}
