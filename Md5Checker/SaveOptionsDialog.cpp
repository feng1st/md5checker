#include "stdafx.h"
#include "SaveOptionsDialog.h"

#include "Options.h"
#include "Resource.h"

INT_PTR CALLBACK CSaveOptionsDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL bInitialized = FALSE;
    static BOOL bChanged = FALSE;
    static COptions *pOptions = NULL;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        bInitialized = FALSE;
        bChanged = FALSE;

        pOptions = (COptions *)((LPPROPSHEETPAGE)lParam)->lParam;

        pOptions->SetChildWnd(MOPSP_SAVE, hwndDlg);
        SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | DS_3DLOOK);

        ::SendMessage(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), EM_LIMITTEXT, MAX_PATH - 1, 0);

        CheckRadioButton(hwndDlg, IDC_FOLDERNAME, IDC_SPECIFY_NAME, pOptions->GetFolderName(TRUE) ? IDC_FOLDERNAME : IDC_SPECIFY_NAME);
        EnableWindow(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), !pOptions->GetFolderName(TRUE));
        SetDlgItemText(hwndDlg, IDC_SPECIFIED_NAME, pOptions->GetDefaultName(TRUE));
        CheckRadioButton(hwndDlg, IDC_ID_SPACE_ASTERISK, IDC_ID_VERTICAL_BAR, IDC_ID_SPACE_ASTERISK + pOptions->GetItemDelimiter(TRUE));
        CheckRadioButton(hwndDlg, IDC_LF_CR_LF, IDC_LF_CR, IDC_LF_CR_LF + pOptions->GetLineFeed(TRUE));
        CheckRadioButton(hwndDlg, IDC_PD_BACKSLASH, IDC_PD_SLASH, IDC_PD_BACKSLASH + pOptions->GetPathDelimiter(TRUE));
        CheckDlgButton(hwndDlg, IDC_SAVE_TO_FULL_PATH, pOptions->GetSaveToFullPath(TRUE));

        bInitialized = TRUE;

        return (INT_PTR)TRUE;

    case WM_NOTIFY:
        switch (((NMHDR*)lParam)->code)
        {
        case PSN_APPLY:
            if (bChanged)
            {
                pOptions->SetFolderName(IsDlgButtonChecked(hwndDlg, IDC_FOLDERNAME) == BST_CHECKED, TRUE);
                GetDlgItemText(hwndDlg, IDC_SPECIFIED_NAME, (LPTSTR)pOptions->GetDefaultName(TRUE), MAX_PATH);
                if (IsDlgButtonChecked(hwndDlg, IDC_ID_SPACES) == BST_CHECKED)
                    pOptions->SetItemDelimiter(MOE_ID_SPACES, TRUE);
                else if (IsDlgButtonChecked(hwndDlg, IDC_ID_VERTICAL_BAR) == BST_CHECKED)
                    pOptions->SetItemDelimiter(MOE_ID_VERTICAL_BAR, TRUE);
                else
                    pOptions->SetItemDelimiter(MOE_ID_SPACE_ASTERISK, TRUE);
                if (IsDlgButtonChecked(hwndDlg, IDC_LF_LF) == BST_CHECKED)
                    pOptions->SetLineFeed(MOE_LF_LF, TRUE);
                else if (IsDlgButtonChecked(hwndDlg, IDC_LF_CR) == BST_CHECKED)
                    pOptions->SetLineFeed(MOE_LF_CR, TRUE);
                else
                    pOptions->SetLineFeed(MOE_LF_CR_LF, TRUE);
                if (IsDlgButtonChecked(hwndDlg, IDC_PD_SLASH) == BST_CHECKED)
                    pOptions->SetPathDelimiter(MOE_PD_SLASH, TRUE);
                else
                    pOptions->SetPathDelimiter(MOE_PD_BACKSLASH, TRUE);
                pOptions->SetSaveToFullPath(IsDlgButtonChecked(hwndDlg, IDC_SAVE_TO_FULL_PATH) == BST_CHECKED, TRUE);
                pOptions->SavePage(MOPSP_SAVE);

                bChanged = FALSE;
            }
            return (INT_PTR)TRUE;
        }
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case MAKEWPARAM(IDC_FOLDERNAME, BN_CLICKED):
        case MAKEWPARAM(IDC_SPECIFY_NAME, BN_CLICKED):
            EnableWindow(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), LOWORD(wParam) == IDC_SPECIFY_NAME);
        case MAKEWPARAM(IDC_SPECIFIED_NAME, EN_CHANGE):
        case MAKEWPARAM(IDC_ID_SPACE_ASTERISK, BN_CLICKED):
        case MAKEWPARAM(IDC_ID_SPACES, BN_CLICKED):
        case MAKEWPARAM(IDC_ID_VERTICAL_BAR, BN_CLICKED):
        case MAKEWPARAM(IDC_LF_CR_LF, BN_CLICKED):
        case MAKEWPARAM(IDC_LF_LF, BN_CLICKED):
        case MAKEWPARAM(IDC_LF_CR, BN_CLICKED):
        case MAKEWPARAM(IDC_PD_BACKSLASH, BN_CLICKED):
        case MAKEWPARAM(IDC_PD_SLASH, BN_CLICKED):
        case MAKEWPARAM(IDC_SAVE_TO_FULL_PATH, BN_CLICKED):
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

CSaveOptionsDialog::CSaveOptionsDialog()
{
}

CSaveOptionsDialog::~CSaveOptionsDialog()
{
}
