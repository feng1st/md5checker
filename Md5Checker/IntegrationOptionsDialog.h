#pragma once

class COptions;

class CIntegrationOptionsDialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CIntegrationOptionsDialog();
    virtual ~CIntegrationOptionsDialog();
};
