#pragma once

class COptions;

class CSaveOptionsDialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CSaveOptionsDialog();
    virtual ~CSaveOptionsDialog();
};
