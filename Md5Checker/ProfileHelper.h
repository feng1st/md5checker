#pragma once

class CProfileHelper
{
private:
    CProfileHelper() { }

public:
    static BOOL WritePrivateProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue, LPCTSTR lpszProfileName);
};
