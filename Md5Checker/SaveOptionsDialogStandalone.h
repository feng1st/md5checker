#pragma once

class CSaveOptionsDialogStandalone
{
public:
    static const int DisableDefaultFileName = 0x01;
    static const int DisablePathDelimiter   = 0x02;

public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CSaveOptionsDialogStandalone();
    virtual ~CSaveOptionsDialogStandalone();

public:
    virtual INT_PTR Show(int nFlag);
};
