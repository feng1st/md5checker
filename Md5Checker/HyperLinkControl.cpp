#include "stdafx.h"
#include "HyperLinkControl.h"

#include "Md5Checker.h"
#include "Resource.h"

#define SM_EDGE     2

WNDPROC CHyperLinkControl::m_wndProcDefault = NULL;

LRESULT CALLBACK CHyperLinkControl::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SETCURSOR)
    {
        SetCursor(LoadCursor(g_hInstance, MAKEINTRESOURCE(IDC_HAND)));
        return (LRESULT)TRUE;
    }

    if (m_wndProcDefault != NULL)
        return CallWindowProc(m_wndProcDefault, hwnd, uMsg, wParam, lParam);
    else
        return (LRESULT)FALSE;
}

CHyperLinkControl::CHyperLinkControl(HWND hWnd)
{
    m_hWnd = hWnd;
    m_hFont = NULL;
    GetWindowText(m_hWnd, m_szText, MAX_CAPTION);

    HDC hDC = GetDC(m_hWnd);
    if (hDC == NULL)
        return;

    if ((m_hFont = (HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0)) != NULL)
    {
        LOGFONT font;
        SecureZeroMemory(&font, sizeof(font));
        GetObject(m_hFont, sizeof(LOGFONT), &font);
        font.lfUnderline = TRUE;
        if ((m_hFont = CreateFontIndirect(&font)) != NULL)
        {
            SelectObject(hDC, m_hFont);
            SendMessage(m_hWnd, WM_SETFONT, (WPARAM)m_hFont, TRUE);

            SIZE szBox;
            GetTextExtentPoint32(hDC, m_szText, (int)_tcslen(m_szText), &szBox);
            szBox.cx += SM_EDGE * 2;
            szBox.cy += SM_EDGE * 2;
            SetWindowPos(m_hWnd, m_hWnd, 0, 0, szBox.cx, szBox.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);

            WNDPROC wndProcPrev = (WNDPROC)(LONG_PTR)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)WindowProc);
            if ((m_wndProcDefault == NULL) && (wndProcPrev != NULL))
                m_wndProcDefault = wndProcPrev;
        }
    }

    ReleaseDC(m_hWnd, hDC);
}

CHyperLinkControl::~CHyperLinkControl()
{
    if (m_hFont != NULL)
        DeleteObject(m_hFont);
}

void CHyperLinkControl::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    HDC hDC = lpDrawItemStruct->hDC;
    LPRECT lpRect = &(lpDrawItemStruct->rcItem);

    SetTextColor(hDC, RGB(0, 0, 0xFF));
    SetBkMode(hDC, TRANSPARENT);
    ExtTextOut(hDC, SM_EDGE, SM_EDGE, ETO_OPAQUE, lpRect, m_szText, (UINT)_tcslen(m_szText), NULL);

    if(GetFocus() == m_hWnd)
        DrawFocusRect(hDC, lpRect);
}

void CHyperLinkControl::Open()
{
    ShellExecute(m_hWnd, _T("open"), m_szText, NULL, NULL, SW_SHOW);
    InvalidateRgn(m_hWnd, NULL, FALSE);
}
