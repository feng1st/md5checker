#include "stdafx.h"
#include "IntegrationOptionsDialog.h"

#include "Options.h"
#include "Resource.h"

INT_PTR CALLBACK CIntegrationOptionsDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        pOptions->SetChildWnd(MOPSP_INTEGRATION, hwndDlg);
        SetWindowLong(hwndDlg, GWL_STYLE, GetWindowLong(hwndDlg, GWL_STYLE) | DS_3DLOOK);

        CheckDlgButton(hwndDlg, IDC_ASSOCIATE, pOptions->GetAssociated());
        CheckDlgButton(hwndDlg, IDC_SHORTCUT, pOptions->IsShortcutAdded());
        CheckDlgButton(hwndDlg, IDC_INTEGRATE, pOptions->GetIntegrated());
        CheckDlgButton(hwndDlg, IDC_SEND_TO, pOptions->IsSendToAdded());

        bInitialized = TRUE;

        return (INT_PTR)TRUE;

    case WM_NOTIFY:
        switch (((NMHDR*)lParam)->code)
        {
        case PSN_APPLY:
            if (bChanged)
            {
                if ((IsDlgButtonChecked(hwndDlg, IDC_ASSOCIATE) == BST_CHECKED) && !pOptions->GetAssociated())
                    pOptions->Associate();
                else if ((IsDlgButtonChecked(hwndDlg, IDC_ASSOCIATE) == BST_UNCHECKED) && pOptions->GetAssociated())
                    pOptions->Unassociate();
                if ((IsDlgButtonChecked(hwndDlg, IDC_SHORTCUT) == BST_CHECKED) && !pOptions->IsShortcutAdded())
                    pOptions->AddShortcut();
                else if ((IsDlgButtonChecked(hwndDlg, IDC_SHORTCUT) == BST_UNCHECKED) && pOptions->IsShortcutAdded())
                    pOptions->RemoveShortcut();
                if ((IsDlgButtonChecked(hwndDlg, IDC_INTEGRATE) == BST_CHECKED) && !pOptions->GetIntegrated())
                    pOptions->Integrate();
                else if ((IsDlgButtonChecked(hwndDlg, IDC_INTEGRATE) == BST_UNCHECKED) && pOptions->GetIntegrated())
                    pOptions->Disintegrate();
                if ((IsDlgButtonChecked(hwndDlg, IDC_SEND_TO) == BST_CHECKED) && !pOptions->IsSendToAdded())
                    pOptions->AddSendTo();
                else if ((IsDlgButtonChecked(hwndDlg, IDC_SEND_TO) == BST_UNCHECKED) && pOptions->IsSendToAdded())
                    pOptions->RemoveSendTo();
                // pOptions->SavePage(MOPSP_INTEGRATION);

                bChanged = FALSE;
            }
            return (INT_PTR)TRUE;
        }
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case MAKEWPARAM(IDC_ASSOCIATE, BN_CLICKED):
        case MAKEWPARAM(IDC_SHORTCUT, BN_CLICKED):
        case MAKEWPARAM(IDC_INTEGRATE, BN_CLICKED):
        case MAKEWPARAM(IDC_SEND_TO, BN_CLICKED):
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

CIntegrationOptionsDialog::CIntegrationOptionsDialog()
{
}

CIntegrationOptionsDialog::~CIntegrationOptionsDialog()
{
}
