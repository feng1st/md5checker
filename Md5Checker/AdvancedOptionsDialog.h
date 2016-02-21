#pragma once

class COptions;

class CAdvancedOptionsDialog
{
public:
    static INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#ifdef UNICODE
    static BOOL CALLBACK EnumCodePagesProc(LPTSTR lpCodePageString);

protected:
    static void LoadCodePages();
#endif

public:
    CAdvancedOptionsDialog();
    virtual ~CAdvancedOptionsDialog();

protected:
    static HWND m_hWndCodePageComboBox;
    static BOOL m_bCodePagesLoaded;
};
