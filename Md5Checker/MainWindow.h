#pragma once

#include "ListView.h"
#include "ReBar.h"
#include "StatusBar.h"

class CMainWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CMainWindow();
    virtual ~CMainWindow();

public:

    virtual HWND GetWnd() const
    {
        return m_hWnd;
    }

    virtual CReBar *GetReBar() const
    {
        return m_pReBar;
    }

    virtual CListView *GetListView() const
    {
        return m_pListView;
    }

    virtual CStatusBar *GetStatusBar() const
    {
        return m_pStatusBar;
    }

public:

    virtual BOOL PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        return ::PostMessage(m_hWnd, Msg, wParam, lParam);
    }

    virtual LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        return ::SendMessage(m_hWnd, Msg, wParam, lParam);
    }

    virtual BOOL GetClientRect(LPRECT lpRect)
    {
        return ::GetClientRect(m_hWnd, lpRect);
    }

    virtual BOOL SetWindowText(LPCTSTR lpszText)
    {
        return ::SetWindowText(m_hWnd, lpszText);
    }

    virtual HWND Create();
    virtual void UpdateToolbar(BOOL bForce = FALSE);
    virtual void UpdateStatusBar(BOOL bForce = FALSE);

public:
    virtual void OnFileExit();
    virtual void OnDestroy();
    virtual void OnItemChanged(LPNMLISTVIEW lpnm);
    virtual void OnODStateChanged(LPNMLVODSTATECHANGE lpnm);
    virtual void OnRClick(LPNMITEMACTIVATE lpnm);
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual void OnSizing(UINT nSide, LPRECT lpRect);
    virtual void OnResize(int cx, int cy);
    virtual void OnProgressBarShow(BOOL bVisible);
    virtual void OnSelectionChanged();
    virtual void OnUpdateViewToolbar();
    virtual void OnUpdateTitle(LPCTSTR lpszPath);

protected:
    HWND m_hWnd;

    CReBar *m_pReBar;
    CListView *m_pListView;
    CStatusBar *m_pStatusBar;
};
