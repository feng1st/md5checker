#pragma once

class CAddOptionsDialogStandalone
{
public:
    static const int DisableAddFolder   = 0x1;

public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CAddOptionsDialogStandalone();
    virtual ~CAddOptionsDialogStandalone();

public:
    virtual INT_PTR Show(int nFlag);
};
