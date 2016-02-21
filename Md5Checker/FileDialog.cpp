#include "stdafx.h"
#include "FileDialog.h"

#include "Md5Checker.h"
#include "Options.h"
#include "Resource.h"

UINT_PTR CALLBACK CFileDialog::AddDialogProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);

    COptions *pOptions = g_app.GetOptions();

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        ::SendMessage(GetDlgItem(hdlg, IDC_INCLUDE), CB_LIMITTEXT, MAX_PATH - 1, 0);
        ::SendMessage(GetDlgItem(hdlg, IDC_EXCLUDE), EM_LIMITTEXT, MAX_PATH - 1, 0);
        CheckDlgButton(hdlg, IDC_AUTO_START, pOptions->GetAutoStart(TRUE));
        EnableWindow(GetDlgItem(hdlg, IDC_CHECK_NEW_ONLY), pOptions->GetAutoStart(TRUE));
        CheckDlgButton(hdlg, IDC_CHECK_NEW_ONLY, pOptions->GetCheckNewOnly(TRUE));
        SendMessage(GetDlgItem(hdlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_COMMON);
        SendMessage(GetDlgItem(hdlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_EXE);
        SendMessage(GetDlgItem(hdlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_ALL);
        SetDlgItemText(hdlg, IDC_INCLUDE, pOptions->GetInclude(TRUE));
        SetDlgItemText(hdlg, IDC_EXCLUDE, pOptions->GetExclude(TRUE));
        CheckDlgButton(hdlg, IDC_RECURSE, pOptions->GetRecurse(TRUE));
        CheckDlgButton(hdlg, IDC_SAVE_SETTINGS, FALSE);
        return (UINT_PTR)TRUE;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case CDN_FOLDERCHANGE:
            // POST
            PostMessage(GetParent(hdlg), CDM_SETCONTROLTEXT, edt1, (LPARAM)pOptions->GetOFNFolder());
            break;

        case CDN_FILEOK:
            if (IsDlgButtonChecked(hdlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SetFlag(COptions::OnceAdd | COptions::OnceCheck, COptions::OnceCheck);
            else
                pOptions->SetFlag(COptions::OnceAdd | COptions::OnceCheck, COptions::OnceAdd | COptions::OnceCheck);
            pOptions->SetAutoStart(IsDlgButtonChecked(hdlg, IDC_AUTO_START) == BST_CHECKED);
            pOptions->SetCheckNewOnly(IsDlgButtonChecked(hdlg, IDC_CHECK_NEW_ONLY) == BST_CHECKED);
            GetDlgItemText(hdlg, IDC_INCLUDE, (LPTSTR)pOptions->GetInclude(), MAX_PATH);
            GetDlgItemText(hdlg, IDC_EXCLUDE, (LPTSTR)pOptions->GetExclude(), MAX_PATH);
            pOptions->SetRecurse(IsDlgButtonChecked(hdlg, IDC_RECURSE) == BST_CHECKED);
            if (IsDlgButtonChecked(hdlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SavePage(MOPSP_ADD);
            break;
        }
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case MAKEWPARAM(IDC_AUTO_START, BN_CLICKED):
            EnableWindow(GetDlgItem(hdlg, IDC_CHECK_NEW_ONLY), IsDlgButtonChecked(hdlg, IDC_AUTO_START) == BST_CHECKED);
            break;
        }
        break;
    }

    return (UINT_PTR)FALSE;
}

UINT_PTR CALLBACK CFileDialog::SaveToDialogProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);

    COptions *pOptions = g_app.GetOptions();

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        CheckRadioButton(hdlg, IDC_ID_SPACE_ASTERISK, IDC_ID_VERTICAL_BAR, IDC_ID_SPACE_ASTERISK + pOptions->GetItemDelimiter(TRUE));
        CheckRadioButton(hdlg, IDC_LF_CR_LF, IDC_LF_CR, IDC_LF_CR_LF + pOptions->GetLineFeed(TRUE));
        CheckRadioButton(hdlg, IDC_PD_BACKSLASH, IDC_PD_SLASH, IDC_PD_BACKSLASH + pOptions->GetPathDelimiter(TRUE));
        CheckDlgButton(hdlg, IDC_SAVE_TO_FULL_PATH, pOptions->GetSaveToFullPath(TRUE));
        CheckDlgButton(hdlg, IDC_SAVE_SETTINGS, FALSE);
        return (UINT_PTR)TRUE;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case CDN_FILEOK:
            if (IsDlgButtonChecked(hdlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SetFlag(COptions::OnceSave, 0);
            else
                pOptions->SetFlag(COptions::OnceSave, COptions::OnceSave);
            if (IsDlgButtonChecked(hdlg, IDC_ID_SPACES) == BST_CHECKED)
                pOptions->SetItemDelimiter(MOE_ID_SPACES);
            else if (IsDlgButtonChecked(hdlg, IDC_ID_VERTICAL_BAR) == BST_CHECKED)
                pOptions->SetItemDelimiter(MOE_ID_VERTICAL_BAR);
            else
                pOptions->SetItemDelimiter(MOE_ID_SPACE_ASTERISK);
            if (IsDlgButtonChecked(hdlg, IDC_LF_LF) == BST_CHECKED)
                pOptions->SetLineFeed(MOE_LF_LF);
            else if (IsDlgButtonChecked(hdlg, IDC_LF_CR) == BST_CHECKED)
                pOptions->SetLineFeed(MOE_LF_CR);
            else
                pOptions->SetLineFeed(MOE_LF_CR_LF);
            if (IsDlgButtonChecked(hdlg, IDC_PD_SLASH) == BST_CHECKED)
                pOptions->SetPathDelimiter(MOE_PD_SLASH);
            else
                pOptions->SetPathDelimiter(MOE_PD_BACKSLASH);
            pOptions->SetSaveToFullPath(IsDlgButtonChecked(hdlg, IDC_SAVE_TO_FULL_PATH) == BST_CHECKED);
            if (IsDlgButtonChecked(hdlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SavePage(MOPSP_SAVE);
            break;
        }
        break;
    }

    return (UINT_PTR)FALSE;
}

CFileDialog::CFileDialog(BOOL bOpenFileDialog, LPTSTR lpszFile, DWORD nMaxFile, DWORD dwFlags,
                         UINT uIDFilter, DWORD nFilterIndex, LPOFNHOOKPROC lpfnHook, UINT uIDTemplate, HWND hParentWnd)
{
    m_bOpenFileDialog = bOpenFileDialog;

    *m_szFilter = 0;
    if (uIDFilter != 0)
    {
        LoadString(g_hInstance, uIDFilter, m_szFilter, MAX_TEXT);
        LPTSTR p = m_szFilter;
        while ((p = _tcschr(p, _T('\n'))) != NULL)
            *p++ = 0;
    }

    SecureZeroMemory(&m_ofn, sizeof(m_ofn));
    m_ofn.lStructSize = sizeof(m_ofn);
    m_ofn.hwndOwner = hParentWnd;
    m_ofn.hInstance = g_hInstance;
    m_ofn.lpstrFilter = (uIDFilter != 0) ? m_szFilter : NULL;
    m_ofn.nFilterIndex = (uIDFilter != 0) ? nFilterIndex : 0;
    m_ofn.lpstrFile = lpszFile;
    m_ofn.nMaxFile = nMaxFile;
    m_ofn.Flags = dwFlags;
    m_ofn.lpfnHook = lpfnHook;
    m_ofn.lpTemplateName = (uIDTemplate != 0) ? MAKEINTRESOURCE(uIDTemplate) : NULL;
}

CFileDialog::~CFileDialog()
{
}

LPCTSTR CFileDialog::Show()
{
    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, szCurrentDir);
    m_ofn.lpstrInitialDir = szCurrentDir;

    if ((m_bOpenFileDialog && GetOpenFileName(&m_ofn)) ||
        (!m_bOpenFileDialog && GetSaveFileName(&m_ofn)))
        return m_ofn.lpstrFile;
    else
        return NULL;
}
