#pragma once

#include "CommonDefine.h"

class CFileDialog
{
public:
    static UINT_PTR CALLBACK AddDialogProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
    static UINT_PTR CALLBACK SaveToDialogProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

public:
    CFileDialog(BOOL bOpenFileDialog, LPTSTR lpszFile, DWORD nMaxFile, DWORD dwFlags,
        UINT uIDFilter, DWORD nFilterIndex, LPOFNHOOKPROC lpfnHook, UINT uIDTemplate, HWND hParentWnd);
    virtual ~CFileDialog();

public:
    virtual LPCTSTR Show();

protected:
    BOOL m_bOpenFileDialog;
    OPENFILENAME m_ofn;
    TCHAR m_szFilter[MAX_TEXT];
};
