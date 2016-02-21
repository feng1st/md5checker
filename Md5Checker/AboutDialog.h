#pragma once

class CAboutDialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CAboutDialog();
    virtual ~CAboutDialog();

public:
    virtual void Show();
};
