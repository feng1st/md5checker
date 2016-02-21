#include "stdafx.h"
#include "AboutDialog.h"

#include "HyperLinkControl.h"
#include "Md5Checker.h"
#include "Resource.h"

INT_PTR CALLBACK CAboutDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static CHyperLinkControl *pHomepageLinkCtrl;
    static CHyperLinkControl *pDownloadLinkCtrl;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        pHomepageLinkCtrl = new CHyperLinkControl(GetDlgItem(hwndDlg, IDC_HOMEPAGE_URL));
        pDownloadLinkCtrl = new CHyperLinkControl(GetDlgItem(hwndDlg, IDC_DOWNLOAD_URL));
        return (INT_PTR)TRUE;

    case WM_DRAWITEM:
        switch (wParam)
        {
        case IDC_HOMEPAGE_URL:
            pHomepageLinkCtrl->Draw((LPDRAWITEMSTRUCT)lParam);
            return (INT_PTR)TRUE;

        case IDC_DOWNLOAD_URL:
            pDownloadLinkCtrl->Draw((LPDRAWITEMSTRUCT)lParam);
            return (INT_PTR)TRUE;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_HOMEPAGE_URL:
            pHomepageLinkCtrl->Open();
            return (INT_PTR)TRUE;

        case IDC_DOWNLOAD_URL:
            pDownloadLinkCtrl->Open();
            return (INT_PTR)TRUE;

        case IDOK:
        case IDCANCEL:
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    case WM_DESTROY:
        delete pHomepageLinkCtrl;
        delete pDownloadLinkCtrl;
        break;
    }

    return (INT_PTR)FALSE;
}

CAboutDialog::CAboutDialog()
{
}

CAboutDialog::~CAboutDialog()
{
}

void CAboutDialog::Show()
{
    DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), g_app.GetMainWnd(), DialogProc);
}
