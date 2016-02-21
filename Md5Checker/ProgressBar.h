#pragma once

#define MAX_PROGRESS_BAR_WIDTH      320

class CProgressBar
{
public:
    CProgressBar(HWND hWndParent);
    virtual ~CProgressBar();

public:

    virtual BOOL IsVisible() const
    {
        return m_bVisible;
    }

public:

    virtual int SetPostion(int nNewPos)
    {
        return (int)SendMessage(m_hWnd, PBM_SETPOS, (WPARAM)nNewPos, 0);
    }

    virtual BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
    {
        return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

    virtual HWND Create();
    virtual void Show(BOOL bVisible = TRUE);

protected:
    HWND m_hWndParent;
    HWND m_hWnd;

    BOOL m_bVisible;
};
