#pragma once

class CRegistryHelper
{
private:
    CRegistryHelper() { }

public:

    static void SetStringValue(HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwType, LPCTSTR lpString)
    {
        SetValue(hKey, lpszSubKey, lpszValueName, dwType, (const BYTE *)lpString, (DWORD)((_tcslen(lpString) + 1) * sizeof(TCHAR)));
    }

    static BOOL QueryValue(HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, LPBYTE lpData, LPDWORD lpcbData);
    static void SetValue(HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, DWORD dwType, const BYTE *lpData, DWORD cbData);
};
