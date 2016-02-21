#include "stdafx.h"
#include "SaveOptionsDialogStandalone.h"

#include "Md5Checker.h"
#include "Options.h"
#include "Resource.h"

INT_PTR CALLBACK CSaveOptionsDialogStandalone::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int nFlag;
    COptions *pOptions = g_app.GetOptions();

    switch (uMsg)
    {
    case WM_INITDIALOG:
        nFlag = (int)lParam;
        if ((nFlag & DisableDefaultFileName) == DisableDefaultFileName)
        {
            EnableWindow(GetDlgItem(hwndDlg, IDC_FOLDERNAME), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_SPECIFY_NAME), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), FALSE);
        }
        else
        {
            ::SendMessage(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), EM_LIMITTEXT, MAX_PATH - 1, 0);
            CheckRadioButton(hwndDlg, IDC_FOLDERNAME, IDC_SPECIFY_NAME, pOptions->GetFolderName(TRUE) ? IDC_FOLDERNAME : IDC_SPECIFY_NAME);
            EnableWindow(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), !pOptions->GetFolderName(TRUE));
            SetDlgItemText(hwndDlg, IDC_SPECIFIED_NAME, pOptions->GetDefaultName(TRUE));
        }
        CheckRadioButton(hwndDlg, IDC_ID_SPACE_ASTERISK, IDC_ID_VERTICAL_BAR, IDC_ID_SPACE_ASTERISK + pOptions->GetItemDelimiter(TRUE));
        CheckRadioButton(hwndDlg, IDC_LF_CR_LF, IDC_LF_CR, IDC_LF_CR_LF + pOptions->GetLineFeed(TRUE));
        if ((nFlag & DisablePathDelimiter) == DisablePathDelimiter)
        {
            EnableWindow(GetDlgItem(hwndDlg, IDC_PD_BACKSLASH), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_PD_SLASH), FALSE);
        }
        else
        {
            CheckRadioButton(hwndDlg, IDC_PD_BACKSLASH, IDC_PD_SLASH, IDC_PD_BACKSLASH + pOptions->GetPathDelimiter(TRUE));
        }
        CheckDlgButton(hwndDlg, IDC_SAVE_SETTINGS, FALSE);
        CheckDlgButton(hwndDlg, IDC_HIDE_SAVE_OPTIONS, pOptions->GetHideSaveOptions());
        SetWindowPos(GetForegroundWindow(), g_app.GetMainWnd(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            if (IsDlgButtonChecked(hwndDlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SetFlag(COptions::OnceSave, 0);
            else
                pOptions->SetFlag(COptions::OnceSave, COptions::OnceSave);
            if ((nFlag & DisableDefaultFileName) != DisableDefaultFileName)
            {
                pOptions->SetFolderName(IsDlgButtonChecked(hwndDlg, IDC_FOLDERNAME) == BST_CHECKED);
                GetDlgItemText(hwndDlg, IDC_SPECIFIED_NAME, (LPTSTR)pOptions->GetDefaultName(), MAX_PATH);
            }
            if (IsDlgButtonChecked(hwndDlg, IDC_ID_SPACES) == BST_CHECKED)
                pOptions->SetItemDelimiter(MOE_ID_SPACES);
            else if (IsDlgButtonChecked(hwndDlg, IDC_ID_VERTICAL_BAR) == BST_CHECKED)
                pOptions->SetItemDelimiter(MOE_ID_VERTICAL_BAR);
            else
                pOptions->SetItemDelimiter(MOE_ID_SPACE_ASTERISK);
            if (IsDlgButtonChecked(hwndDlg, IDC_LF_LF) == BST_CHECKED)
                pOptions->SetLineFeed(MOE_LF_LF);
            else if (IsDlgButtonChecked(hwndDlg, IDC_LF_CR) == BST_CHECKED)
                pOptions->SetLineFeed(MOE_LF_CR);
            else
                pOptions->SetLineFeed(MOE_LF_CR_LF);
            if ((nFlag & DisablePathDelimiter) != DisablePathDelimiter)
            {
                if (IsDlgButtonChecked(hwndDlg, IDC_PD_SLASH) == BST_CHECKED)
                    pOptions->SetPathDelimiter(MOE_PD_SLASH);
                else
                    pOptions->SetPathDelimiter(MOE_PD_BACKSLASH);
            }
            if (IsDlgButtonChecked(hwndDlg, IDC_SAVE_SETTINGS) == BST_CHECKED)
                pOptions->SavePage(MOPSP_SAVE);
            if ((IsDlgButtonChecked(hwndDlg, IDC_HIDE_SAVE_OPTIONS) == BST_CHECKED) != pOptions->GetHideSaveOptions())
            {
                pOptions->SetHideSaveOptions(IsDlgButtonChecked(hwndDlg, IDC_HIDE_SAVE_OPTIONS) == BST_CHECKED);
                pOptions->SaveSingle(IDC_HIDE_SAVE_OPTIONS);
            }
        case IDCANCEL:
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        default:
            switch (wParam)
            {
            case MAKEWPARAM(IDC_FOLDERNAME, BN_CLICKED):
            case MAKEWPARAM(IDC_SPECIFY_NAME, BN_CLICKED):
                if ((nFlag & DisableDefaultFileName) != DisableDefaultFileName)
                    EnableWindow(GetDlgItem(hwndDlg, IDC_SPECIFIED_NAME), LOWORD(wParam) == IDC_SPECIFY_NAME);
                break;
            }
        }
        break;
    }

    return (INT_PTR)FALSE;
}

CSaveOptionsDialogStandalone::CSaveOptionsDialogStandalone()
{
}

CSaveOptionsDialogStandalone::~CSaveOptionsDialogStandalone()
{
}

INT_PTR CSaveOptionsDialogStandalone::Show(int nFlag)
{
    COptions *pOptions = g_app.GetOptions();

    if (!pOptions->GetHideSaveOptions() || ((GetAsyncKeyState(VK_SHIFT) & ~1) != 0))
        return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_SAVE_OPTIONS_STANDALONE), g_app.GetMainWnd(), DialogProc, (LPARAM)nFlag);
    else
        return (INT_PTR)IDOK;
}
