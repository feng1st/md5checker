#pragma once

class CCalculatorDialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CCalculatorDialog();
    virtual ~CCalculatorDialog();

public:
    virtual void Show();

protected:
    static void CalculateMD5(HWND hwndDlg);
    static void CopyMD5(HWND hwndDlg);
};
