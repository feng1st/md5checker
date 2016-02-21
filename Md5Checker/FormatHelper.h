#pragma once

class CFormatHelper
{
private:
    CFormatHelper() { }

public:
    static LPCTSTR ToHexString(BYTE bytes[], int n, LPTSTR lpszString);
    static BOOL IsValidMD5(LPCTSTR lpszMD5);
    static BOOL ReadMD5(LPCTSTR &lpszSrc, LPTSTR lpszMD5);
    static BOOL ReadDelimiter(LPCTSTR &lpszSrc);
};
