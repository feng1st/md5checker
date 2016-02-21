#include "stdafx.h"
#include "CompareMD5Dialog.h"

#include "ClipboardHelper.h"
#include "CommonDefine.h"
#include "FormatHelper.h"
#include "Md5Checker.h"
#include "Md5Item.h"
#include "Resource.h"

HWND CCompareMD5Dialog::m_hWnd = NULL;

INT_PTR CALLBACK CCompareMD5Dialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TCHAR szMD51[MAX_MD5];
    TCHAR szMD52[MAX_MD5];
    TCHAR szText[MAX_CAPTION];
    CMd5Item *pMd5Item1, *pMd5Item2;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        ::SendMessage(GetDlgItem(hwndDlg, IDC_MD5_2), EM_LIMITTEXT, SIZE_MD5, 0);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (wParam)
        {
        case MAKEWPARAM(IDC_MD5_2, EN_CHANGE):
            GetDlgItemText(hwndDlg, IDC_MD5_1, szMD51, MAX_MD5);
            GetDlgItemText(hwndDlg, IDC_MD5_2, szMD52, MAX_MD5);
            if (CFormatHelper::IsValidMD5(szMD51) && CFormatHelper::IsValidMD5(szMD52))
            {
                if (_tcsicmp(szMD51, szMD52) == 0)
                    LoadString(g_hInstance, IDS_SAME, szText, MAX_CAPTION);
                else
                    LoadString(g_hInstance, IDS_DIFFERENT, szText, MAX_CAPTION);
                SetDlgItemText(hwndDlg, IDC_COMPARE_RESULT, szText);
            }
            else
            {
                SetDlgItemText(hwndDlg, IDC_COMPARE_RESULT, NULL);
            }
            break;
        case MAKEWPARAM(IDC_PASTE_MD5, BN_CLICKED):
            if (CClipboardHelper::Paste(g_app.GetMainWnd(), szMD52, MAX_MD5))
                SetDlgItemText(hwndDlg, IDC_MD5_2, szMD52);
            break;
        case MAKEWPARAM(IDCLOSE, BN_CLICKED):
        case MAKEWPARAM(IDCANCEL, BN_CLICKED):
            DestroyWindow(hwndDlg);
            return (INT_PTR)TRUE;
        }
        break;

    case WM_DESTROY:
        m_hWnd = NULL;
        break;

    case WM_USER_COMPARE_MD5:
        pMd5Item1 = (CMd5Item *)wParam;
        pMd5Item2 = (CMd5Item *)lParam;
        SetDlgItemText(hwndDlg, IDC_FILE_PATH_1, pMd5Item1->GetFullPath());
        SetDlgItemText(hwndDlg, IDC_MD5_1, pMd5Item1->GetCurrentMD5());
        if (pMd5Item2 != NULL)
        {
            ::SendMessage(GetDlgItem(hwndDlg, IDC_MD5_2), EM_SETREADONLY, TRUE, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_FILE_PATH_2), TRUE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_PASTE_MD5), FALSE);
            SetDlgItemText(hwndDlg, IDC_FILE_PATH_2, pMd5Item2->GetFullPath());
            SetDlgItemText(hwndDlg, IDC_MD5_2, pMd5Item2->GetCurrentMD5());
        }
        else
        {
            ::SendMessage(GetDlgItem(hwndDlg, IDC_MD5_2), EM_SETREADONLY, FALSE, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_FILE_PATH_2), FALSE);
            EnableWindow(GetDlgItem(hwndDlg, IDC_PASTE_MD5), TRUE);
            LoadString(g_hInstance, IDS_NONE, szText, MAX_CAPTION);
            SetDlgItemText(hwndDlg, IDC_FILE_PATH_2, szText);
            SetDlgItemText(hwndDlg, IDC_MD5_2, NULL);
        }
        break;
    }

    return (INT_PTR)FALSE;
}

CCompareMD5Dialog::CCompareMD5Dialog()
{
}

CCompareMD5Dialog::~CCompareMD5Dialog()
{
}

void CCompareMD5Dialog::Show()
{
    if (m_hWnd == NULL)
    {
        m_hWnd = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_COMPARE_MD5), g_app.GetMainWnd(), DialogProc);
        ShowWindow(m_hWnd, SW_SHOW);
    }
}
