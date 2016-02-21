#include "stdafx.h"
#include "AdvancedOptionsDialog.h"

#include "Md5Checker.h"
#include "Options.h"
#include "Resource.h"
#include "ShellHelper.h"

HWND CAdvancedOptionsDialog::m_hWndCodePageComboBox = NULL;
BOOL CAdvancedOptionsDialog::m_bCodePagesLoaded = FALSE;

INT_PTR CALLBACK CAdvancedOptionsDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL bInitialized = FALSE;
    static BOOL bChanged = FALSE;
    static COptions *pOptions = NULL;
#ifdef UNICODE
    TCHAR szDefaultCodePageName[MAX_PATH];
    TCHAR szCodePageName[MAX_PATH];
    int i;
    LRESULT iCurSel, iCodePage;
#endif

    switch (uMsg)
    {
    case WM_INITDIALOG:
        bInitialized = FALSE;
        bChanged = FALSE;

        pOptions = (COptions *)((LPPROPSHEETPAGE)lParam)->lParam;

        pOptions->SetChildWnd(MOPSP_ADVANCED, hwndDlg);
        SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | DS_3DLOOK);

        CheckDlgButton(hwndDlg, IDC_LOW_PRIORITY, pOptions->GetLowPriority());
        CheckDlgButton(hwndDlg, IDC_STORE_LFU, pOptions->GetStoreLFU());
        m_hWndCodePageComboBox = GetDlgItem(hwndDlg, IDC_CODE_PAGE);
#ifdef UNICODE
        LoadString(g_hInstance, IDS_DEFAULT_CODE_PAGE_NAME, szDefaultCodePageName, MAX_PATH);
        if ((i = (int)SendMessage(m_hWndCodePageComboBox, CB_ADDSTRING, 0, (LPARAM)szDefaultCodePageName)) > 0)
            SendMessage(m_hWndCodePageComboBox, CB_SETITEMDATA, i, MOD_CODE_PAGE);
        if (CShellHelper::GetCodePageName(pOptions->GetCodePage(), szCodePageName))
        {
            if ((i = (int)SendMessage(m_hWndCodePageComboBox, CB_ADDSTRING, 0, (LPARAM)szCodePageName)) > 0)
                SendMessage(m_hWndCodePageComboBox, CB_SETITEMDATA, i, pOptions->GetCodePage());
            SendMessage(m_hWndCodePageComboBox, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)szCodePageName);
        }
        else
        {
            SendMessage(m_hWndCodePageComboBox, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)szDefaultCodePageName);
        }
        m_bCodePagesLoaded = FALSE;
#else
        EnableWindow(m_hWndCodePageComboBox, FALSE);
#endif
        CheckDlgButton(hwndDlg, IDC_HIDE_ADD_OPTIONS, pOptions->GetHideAddOptions());
        CheckDlgButton(hwndDlg, IDC_HIDE_SAVE_OPTIONS, pOptions->GetHideSaveOptions());

        bInitialized = TRUE;

        return (INT_PTR)TRUE;

    case WM_NOTIFY:
        switch (((NMHDR*)lParam)->code)
        {
        case PSN_APPLY:
            if (bChanged)
            {
                pOptions->SetLowPriority(IsDlgButtonChecked(hwndDlg, IDC_LOW_PRIORITY) == BST_CHECKED);
                pOptions->SetStoreLFU(IsDlgButtonChecked(hwndDlg, IDC_STORE_LFU) == BST_CHECKED);
#ifdef UNICODE
                if (((iCurSel = SendMessage(m_hWndCodePageComboBox, CB_GETCURSEL, 0, 0)) != CB_ERR) &&
                    ((iCodePage = SendMessage(m_hWndCodePageComboBox, CB_GETITEMDATA, iCurSel, 0)) != CB_ERR))
                    pOptions->SetCodePage((UINT)iCodePage);
                else
                    pOptions->SetCodePage(MOD_CODE_PAGE);
#endif
                pOptions->SetHideAddOptions(IsDlgButtonChecked(hwndDlg, IDC_HIDE_ADD_OPTIONS) == BST_CHECKED);
                pOptions->SetHideSaveOptions(IsDlgButtonChecked(hwndDlg, IDC_HIDE_SAVE_OPTIONS) == BST_CHECKED);
                pOptions->SavePage(MOPSP_ADVANCED);

                bChanged = FALSE;
            }
            return (INT_PTR)TRUE;
        }
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case MAKEWPARAM(IDC_DEFAULT, BN_CLICKED):
            pOptions->ResetDialog();
        case MAKEWPARAM(IDC_LOW_PRIORITY, BN_CLICKED):
        case MAKEWPARAM(IDC_STORE_LFU, BN_CLICKED):
#ifdef UNICODE
        case MAKEWPARAM(IDC_CODE_PAGE, CBN_SELCHANGE):
#endif
        case MAKEWPARAM(IDC_HIDE_ADD_OPTIONS, BN_CLICKED):
        case MAKEWPARAM(IDC_HIDE_SAVE_OPTIONS, BN_CLICKED):
            if (bInitialized)
            {
                bChanged = TRUE;
                PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
            }
            break;
#ifdef UNICODE
        case MAKEWPARAM(IDC_CODE_PAGE, CBN_DROPDOWN):
            LoadCodePages();
            break;
#endif
        }
        break;
    }

    return (INT_PTR)FALSE;
}

#ifdef UNICODE
BOOL CALLBACK CAdvancedOptionsDialog::EnumCodePagesProc(LPTSTR lpCodePageString)
{
    UINT iCodePage = _ttoi(lpCodePageString);
    TCHAR szCodePageName[MAX_PATH];
    if (CShellHelper::GetCodePageName(iCodePage, szCodePageName))
    {
        int i;
        if ((i = (int)SendMessage(m_hWndCodePageComboBox, CB_ADDSTRING, 0, (LPARAM)szCodePageName)) > 0)
            SendMessage(m_hWndCodePageComboBox, CB_SETITEMDATA, i, iCodePage);
    }

    return TRUE;
}

void CAdvancedOptionsDialog::LoadCodePages()
{
    if (!m_bCodePagesLoaded)
    {
        m_bCodePagesLoaded = TRUE;
        EnumSystemCodePages(EnumCodePagesProc, CP_INSTALLED);
    }
}
#endif

CAdvancedOptionsDialog::CAdvancedOptionsDialog()
{
}

CAdvancedOptionsDialog::~CAdvancedOptionsDialog()
{
}
