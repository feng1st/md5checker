#include "stdafx.h"
#include "MessageBoxHelper.h"

#include "CommonDefine.h"

int CMessageBoxHelper::Show(HINSTANCE hInstance, HWND hWnd, UINT uIDText, UINT uIDCaption, UINT uType)
{
    TCHAR szCaption[MAX_CAPTION];
    TCHAR szText[MAX_TEXT];

    LoadString(hInstance, uIDCaption, szCaption, MAX_CAPTION);
    LoadString(hInstance, uIDText, szText, MAX_TEXT);

    return MessageBox(hWnd, szText, szCaption, uType);
}

int CMessageBoxHelper::Show(HINSTANCE hInstance, HWND hWnd, UINT uIDText, UINT uIDCaption, UINT uType, LPCTSTR lpszString1)
{
    TCHAR szCaption[MAX_CAPTION];
    TCHAR szFormat[MAX_TEXT];
    TCHAR szText[MAX_TEXT];

    LoadString(hInstance, uIDCaption, szCaption, MAX_CAPTION);
    LoadString(hInstance, uIDText, szFormat, MAX_TEXT);
    _stprintf_s(szText, MAX_TEXT, szFormat, lpszString1);

    return MessageBox(hWnd, szText, szCaption, uType);
}
