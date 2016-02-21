#include "stdafx.h"
#include "AddOptionsDialog.h"

#include "Options.h"
#include "Resource.h"
#include "WindowHelper.h"

INT_PTR CALLBACK CAddOptionsDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL bInitialized = FALSE;
    static BOOL bChanged = FALSE;
    static COptions *pOptions = NULL;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        bInitialized = FALSE;
        bChanged = FALSE;

        CWindowHelper::CenterWindow(GetParent(hwndDlg));

        pOptions = (COptions *)((LPPROPSHEETPAGE)lParam)->lParam;

        pOptions->SetChildWnd(MOPSP_ADD, hwndDlg);
        SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | DS_3DLOOK);

        ::SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_LIMITTEXT, MAX_PATH - 1, 0);
        ::SendMessage(GetDlgItem(hwndDlg, IDC_EXCLUDE), EM_LIMITTEXT, MAX_PATH - 1, 0);

        CheckDlgButton(hwndDlg, IDC_AUTO_START, pOptions->GetAutoStart(TRUE));
        SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_COMMON);
        SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_EXE);
        SendMessage(GetDlgItem(hwndDlg, IDC_INCLUDE), CB_ADDSTRING, 0, (LPARAM)MOD_INCLUDE_ALL);
        SetDlgItemText(hwndDlg, IDC_INCLUDE, pOptions->GetInclude(TRUE));
        SetDlgItemText(hwndDlg, IDC_EXCLUDE, pOptions->GetExclude(TRUE));
        CheckDlgButton(hwndDlg, IDC_RECURSE, pOptions->GetRecurse(TRUE));

        bInitialized = TRUE;

        return (INT_PTR)TRUE;

    case WM_NOTIFY:
        switch (((NMHDR*)lParam)->code)
        {
        case PSN_APPLY:
            if (bChanged)
            {
                pOptions->SetAutoStart(IsDlgButtonChecked(hwndDlg, IDC_AUTO_START) == BST_CHECKED, TRUE);
                GetDlgItemText(hwndDlg, IDC_INCLUDE, (LPTSTR)pOptions->GetInclude(TRUE), MAX_PATH);
                GetDlgItemText(hwndDlg, IDC_EXCLUDE, (LPTSTR)pOptions->GetExclude(TRUE), MAX_PATH);
                pOptions->SetRecurse(IsDlgButtonChecked(hwndDlg, IDC_RECURSE) == BST_CHECKED, TRUE);
                pOptions->SavePage(MOPSP_ADD);

                bChanged = FALSE;
            }
            return (INT_PTR)TRUE;
        }
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case MAKEWPARAM(IDC_AUTO_START, BN_CLICKED):
        case MAKEWPARAM(IDC_INCLUDE, CBN_EDITCHANGE):
        case MAKEWPARAM(IDC_INCLUDE, CBN_SELCHANGE):
        case MAKEWPARAM(IDC_EXCLUDE, EN_CHANGE):
        case MAKEWPARAM(IDC_RECURSE, BN_CLICKED):
            if (bInitialized)
            {
                bChanged = TRUE;
                PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
            }
            break;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

CAddOptionsDialog::CAddOptionsDialog()
{
}

CAddOptionsDialog::~CAddOptionsDialog()
{
}
