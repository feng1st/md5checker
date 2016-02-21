#pragma once

#include "MainToolbar.h"
#include "ViewToolbar.h"

class CReBar
{
public:
    CReBar(HWND hWndParent);
    virtual ~CReBar();

public:

    virtual CMainToolbar *GetMainToolbar() const
    {
        return m_pMainToolbar;
    }

    virtual CViewToolbar *GetViewToolbar() const
    {
        return m_pViewToolbar;
    }

public:

    virtual HDWP DeferWindowPos(HDWP hWinPosInfo, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags)
    {
        return ::DeferWindowPos(hWinPosInfo, m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
    }

    virtual BOOL GetClientRect(LPRECT lpRect)
    {
        return ::GetClientRect(m_hWnd, lpRect);
    }

    virtual BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
    {
        return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    virtual HWND Create();

protected:
    HWND m_hWndParent;
    HWND m_hWnd;

    CMainToolbar *m_pMainToolbar;
    CViewToolbar *m_pViewToolbar;
};
