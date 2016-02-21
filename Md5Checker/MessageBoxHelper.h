#pragma once

class CMessageBoxHelper
{
private:
    CMessageBoxHelper() { }

public:
    static int Show(HINSTANCE hInstance, HWND hWnd, UINT uIDText, UINT uIDCaption, UINT uType);
    static int Show(HINSTANCE hInstance, HWND hWnd, UINT uIDText, UINT uIDCaption, UINT uType, LPCTSTR lpszString1);
};
