#include "stdafx.h"
#include "RegistryHelper.h"

BOOL CRegistryHelper::QueryValue(HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPBYTE lpData, LPDWORD lpcbData)
{
    HKEY hk;

    if (RegOpenKeyEx(hKey, lpszSubKey, 0, KEY_QUERY_VALUE, &hk) != ERROR_SUCCESS)
        return FALSE;

    if (RegQueryValueEx(hk, lpszValueName, NULL, NULL, lpData, lpcbData) == ERROR_SUCCESS)
    {
        RegCloseKey(hk);
        return TRUE;
    }
    else
    {
        RegCloseKey(hk);
        return FALSE;
    }
}

void CRegistryHelper::SetValue(HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwType, const BYTE *lpData, DWORD cbData)
{
    HKEY hk;

    if (RegCreateKeyEx(hKey, lpszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hk, NULL) != ERROR_SUCCESS)
        return;

    RegSetValueEx(hk, lpszValueName, 0, dwType, lpData, cbData);

    RegCloseKey(hk);
}
