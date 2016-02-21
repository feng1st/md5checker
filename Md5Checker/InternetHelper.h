#pragma once

class CInternetHelper
{
private:
    CInternetHelper() { }

public:
    static BOOL ReadA(LPCTSTR lpszURL, LPCTSTR lpszAgent, LPSTR lpszString, UINT nString);
};
