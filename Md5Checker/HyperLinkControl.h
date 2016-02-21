#pragma once

#include "CommonDefine.h"

class CHyperLinkControl
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CHyperLinkControl(HWND hWnd);
    virtual ~CHyperLinkControl();

public:
    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void Open();

protected:
    static WNDPROC m_wndProcDefault;

    HWND m_hWnd;
    HFONT m_hFont;
    TCHAR m_szText[MAX_CAPTION];
};
