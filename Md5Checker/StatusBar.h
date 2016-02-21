#pragma once

#include "CommonDefine.h"
#include "ProgressBar.h"

class CStatusBar
{
public:
    CStatusBar(HWND hWndParent);
    virtual ~CStatusBar();

public:

    virtual CProgressBar *GetProgressBar() const
    {
        return m_pProgressBar;
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

    virtual BOOL SetText(LPCTSTR lpszText)
    {
        return (BOOL)SendMessage(m_hWnd, SB_SETTEXT, 0, (LPARAM)lpszText);
    }

    virtual BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
    {
        return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    virtual HWND Create();
    virtual void UpdateParts();
    virtual void SetTextUnselected(int nTotalListed);
    virtual void SetTextSelected(int nSelected);
    virtual void SetTextAdding(LPTSTR lpszPath);
    virtual void SetTextChecking(int nChecked, int nTotal, LPCTSTR lpszFilePath);

protected:
    HWND m_hWndParent;
    HWND m_hWnd;

    CProgressBar *m_pProgressBar;

    TCHAR m_szUnselected[MAX_TEXT];
    TCHAR m_szSelected[MAX_TEXT];
    TCHAR m_szAdding[MAX_TEXT];
    TCHAR m_szChecking[MAX_TEXT];
};
