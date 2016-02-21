#include "stdafx.h"
#include "AddOptionsDialogStandalone.h"

#include "Md5Checker.h"
#include "Options.h"
#include "Resource.h"

INT_PTR CALLBACK CAddOptionsDialogStandalone::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int nFlag;
    COptions *pOptions = g_app.GetOptions();

    switch (uMsg)
    {
    case WM_INITDIALOG:
        nFlag = (int)lParam;
        CheckDlgButton(hwndDlg, IDC_AUTO_START, pOptions->GetAutoStart(TRUE));
        EnableWindow(GetDlgItem(hwndDlg, IDC_CHECK_NEW_ONLY), pOptions->GetAutoStart(TRUE));
        CheckDlgButton(hwndDlg, IDC_CHECK_NEW_ONLY, pOptions->GetCheckNewOnly(TRUE));
        if ((nFlag & DisableAddFolder) == DisableAddFolder)
        {
            EnableWindow(GetDlgItem(hwndDlg, IDC_INCLUDE), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_EXCLUDE), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_RECURSE), FALSE);
        }
        else
        {
            ::SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_LIMITTEXT, MAX_PATH - 1, 0);
            ::SendMessage(GetDlgItem(hwndDlg, IDC_EXCLUDE), EM_LIMITTEXT, MAX_PATH - 1, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_COMMON);
            SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_EXE);
            SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_ALL);
            SetDlgItemText(hwndDlg, IDC_INCLUDE, pOptions->GetInclude(TRUE));
            SetDlgItemText(hwndDlg, IDC_EXCLUDE, pOptions->GetExclude(TRUE));
            CheckDlgButton(hwndDlg, IDC_RECURSE, pOptions->GetRecurse(TRUE));
        }
        CheckDlgButton(hwndDlg, IDC_SAVE_SETTINGS, FALSE);
        CheckDlgButton(hwndDlg, IDC_HIDE_ADD_OPTIONS, pOptions->GetHideAddOptions());
        SetWindowPos(GetForegroundWindow(), g_app.GetMainWnd(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            if (IsDlgButtonChecked(hwndDlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SetFlag(COptions::OnceAdd | COptions::OnceCheck, COptions::OnceCheck);
            else
                pOptions->SetFlag(COptions::OnceAdd | COptions::OnceCheck, COptions::OnceAdd | COptions::OnceCheck);
            pOptions->SetAutoStart(IsDlgButtonChecked(hwndDlg, IDC_AUTO_START) == BST_CHECKED);
            pOptions->SetCheckNewOnly(IsDlgButtonChecked(hwndDlg, IDC_CHECK_NEW_ONLY) == BST_CHECKED);
            if ((nFlag & DisableAddFolder) != DisableAddFolder)
            {
                GetDlgItemText(hwndDlg, IDC_INCLUDE, (LPTSTR)pOptions->GetInclude(), MAX_PATH);
                GetDlgItemText(hwndDlg, IDC_EXCLUDE, (LPTSTR)pOptions->GetExclude(), MAX_PATH);
                pOptions->SetRecurse(IsDlgButtonChecked(hwndDlg, IDC_RECURSE) == BST_CHECKED);
            }
            if (IsDlgButtonChecked(hwndDlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SavePage(MOPSP_ADD);
            if ((IsDlgButtonChecked(hwndDlg, IDC_HIDE_ADD_OPTIONS) == BST_CHECKED) != pOptions->GetHideAddOptions())
            {
                pOptions->SetHideAddOptions(IsDlgButtonChecked(hwndDlg, IDC_HIDE_ADD_OPTIONS) == BST_CHECKED);
                pOptions->SaveSingle(IDC_HIDE_ADD_OPTIONS);
            }
        case IDCANCEL:
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        default:
            switch (wParam)
            {
            case MAKEWPARAM(IDC_AUTO_START, BN_CLICKED):
                EnableWindow(GetDlgItem(hwndDlg, IDC_CHECK_NEW_ONLY), IsDlgButtonChecked(hwndDlg, IDC_AUTO_START) == BST_CHECKED);
                break;
            }
        }
        break;
    }

    return (INT_PTR)FALSE;
}

CAddOptionsDialogStandalone::CAddOptionsDialogStandalone()
{
}

CAddOptionsDialogStandalone::~CAddOptionsDialogStandalone()
{
}

INT_PTR CAddOptionsDialogStandalone::Show(int nFlag)
{
    COptions *pOptions = g_app.GetOptions();

    if (!pOptions->GetHideAddOptions() || ((GetAsyncKeyState(VK_SHIFT) & ~1) != 0))
        return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ADD_OPTIONS_STANDALONE), g_app.GetMainWnd(), DialogProc, (LPARAM)nFlag);
    else
        return (INT_PTR)IDOK;
}
