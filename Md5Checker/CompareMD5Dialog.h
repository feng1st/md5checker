#pragma once

class CCompareMD5Dialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CCompareMD5Dialog();
    virtual ~CCompareMD5Dialog();

public:

    virtual LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        if (m_hWnd != NULL)
            return ::SendMessage(m_hWnd, Msg, wParam, lParam);
        else
            return 0;
    }

    virtual BOOL IsDialogMessage(LPMSG lpMsg)
    {
        return (m_hWnd != NULL) && ::IsDialogMessage(m_hWnd, lpMsg);
    }

    virtual void Show();

protected:
    static HWND m_hWnd;
};
