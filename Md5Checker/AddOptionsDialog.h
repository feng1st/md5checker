#pragma once

class CAddOptionsDialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CAddOptionsDialog();
    virtual ~CAddOptionsDialog();
};
