#pragma once

class CClipboardHelper
{
private:
    CClipboardHelper() { }

public:
    static BOOL Copy(HWND hWnd, LPCTSTR lpszText);
    static BOOL Paste(HWND hWnd, LPTSTR lpszText, int nText);
};
