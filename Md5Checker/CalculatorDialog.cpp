#include "stdafx.h"
#include "CalculatorDialog.h"

#include "ClipboardHelper.h"
#include "CommonDefine.h"
#include "FormatHelper.h"
#include "MD5Calculator.h"
#include "Md5Checker.h"
#include "Md5Item.h"
#include "Resource.h"

INT_PTR CALLBACK CCalculatorDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
#ifndef UNICODE
        EnableWindow(GetDlgItem(hwndDlg, IDC_UNICODE), FALSE);
#endif
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CALCULATE_MD5:
            CalculateMD5(hwndDlg);
            return (INT_PTR)TRUE;

        case IDC_COPY_MD5:
            CopyMD5(hwndDlg);
            return (INT_PTR)TRUE;

        case IDCLOSE:
        case IDCANCEL:
            EndDialog(hwndDlg, IDCLOSE);
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

CCalculatorDialog::CCalculatorDialog()
{
}

CCalculatorDialog::~CCalculatorDialog()
{
}

void CCalculatorDialog::Show()
{
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_MD5_CALCULATOR), g_app.GetMainWnd(), DialogProc);
}

void CCalculatorDialog::CalculateMD5(HWND hwndDlg)
{
    CMD5Calculator md5Calculator;
    CHAR aszText[MAX_EDIT_TEXT];
#ifdef UNICODE
    WCHAR wszText[MAX_EDIT_TEXT];
    size_t stConverted;
#endif
    BYTE byMD5[BYTES_MD5];
    TCHAR szMD5[MAX_MD5];

    md5Calculator.Init();
#ifdef UNICODE
    GetDlgItemTextW(hwndDlg, IDC_TEXT_INPUT, wszText, MAX_EDIT_TEXT);
    if (IsDlgButtonChecked(hwndDlg, IDC_UNICODE) == BST_CHECKED)
    {
        md5Calculator.Update((const LPBYTE)wszText, (UINT)(wcslen(wszText) * sizeof(WCHAR)));
    }
    else
    {
        wcstombs_s(&stConverted, aszText, MAX_EDIT_TEXT, wszText, MAX_EDIT_TEXT);
        md5Calculator.Update((const LPBYTE)aszText, (UINT)strlen(aszText));
    }
#else
    GetDlgItemTextA(hwndDlg, IDC_TEXT_INPUT, aszText, MAX_EDIT_TEXT);
    /*
    if (IsDlgButtonChecked(hwndDlg, IDC_UNICODE) == BST_CHECKED)
    {
        mbstowcs_s(&stConverted, wszText, MAX_EDIT_TEXT, aszText, MAX_EDIT_TEXT);
        md5Calculator.Update((const LPBYTE)wszText, (UINT)(wcslen(wszText) * sizeof(WCHAR)));
    }
    else
    {
    */
    md5Calculator.Update((const LPBYTE)aszText, (UINT)strlen(aszText));
    /*
    }
    */
#endif
    md5Calculator.Finalize(byMD5);
    SetDlgItemText(hwndDlg, IDC_MD5_OUTPUT, CFormatHelper::ToHexString(byMD5, BYTES_MD5, szMD5));
}

void CCalculatorDialog::CopyMD5(HWND hwndDlg)
{
    TCHAR szMD5[MAX_MD5];
    GetDlgItemText(hwndDlg, IDC_MD5_OUTPUT, szMD5, MAX_MD5);
    CClipboardHelper::Copy(g_app.GetMainWnd(), szMD5);
}
