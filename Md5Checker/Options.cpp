#include "stdafx.h"
#include "Options.h"

#include <locale.h>

#include "AddOptionsDialog.h"
#include "AdvancedOptionsDialog.h"
#include "IntegrationOptionsDialog.h"
#include "Md5Checker.h"
#include "Path.h"
#include "ProfileHelper.h"
#include "Resource.h"
#include "SaveOptionsDialog.h"
#include "ShellHelper.h"

int CALLBACK COptions::PropSheetProc(HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    if (uMsg == PSCB_INITIALIZED)
    {
        for (int i = 1; i < MAX_OPTION_PSPS; i++)
            PropSheet_SetCurSel(hwndDlg, NULL, i);
    }

    return 0;
}

COptions::COptions()
{
    m_nFlag = 0;

    InitStrings();
    Load();
    LoadLastFolderUsed();
}

COptions::~COptions()
{
    SaveLastFolderUsed();
}

void COptions::Show()
{
    PROPSHEETPAGE psps[MAX_OPTION_PSPS];

    psps[MOPSP_ADD].dwSize = sizeof(PROPSHEETPAGE);
    psps[MOPSP_ADD].dwFlags = 0;
    psps[MOPSP_ADD].hInstance = g_hInstance;
    psps[MOPSP_ADD].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_ADD);
    psps[MOPSP_ADD].pfnDlgProc = CAddOptionsDialog::DialogProc;
    psps[MOPSP_ADD].lParam = (LPARAM)this;

    psps[MOPSP_SAVE].dwSize = sizeof(PROPSHEETPAGE);
    psps[MOPSP_SAVE].dwFlags = 0;
    psps[MOPSP_SAVE].hInstance = g_hInstance;
    psps[MOPSP_SAVE].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_SAVE);
    psps[MOPSP_SAVE].pfnDlgProc = CSaveOptionsDialog::DialogProc;
    psps[MOPSP_SAVE].lParam = (LPARAM)this;

    psps[MOPSP_INTEGRATION].dwSize = sizeof(PROPSHEETPAGE);
    psps[MOPSP_INTEGRATION].dwFlags = 0;
    psps[MOPSP_INTEGRATION].hInstance = g_hInstance;
    psps[MOPSP_INTEGRATION].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_INTEGRATION);
    psps[MOPSP_INTEGRATION].pfnDlgProc = CIntegrationOptionsDialog::DialogProc;
    psps[MOPSP_INTEGRATION].lParam = (LPARAM)this;

    psps[MOPSP_ADVANCED].dwSize = sizeof(PROPSHEETPAGE);
    psps[MOPSP_ADVANCED].dwFlags = 0;
    psps[MOPSP_ADVANCED].hInstance = g_hInstance;
    psps[MOPSP_ADVANCED].pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_ADVANCED);
    psps[MOPSP_ADVANCED].pfnDlgProc = CAdvancedOptionsDialog::DialogProc;
    psps[MOPSP_ADVANCED].lParam = (LPARAM)this;

    TCHAR szCaption[MAX_CAPTION];
    LoadString(g_hInstance, IDS_OPTIONS_CAPTION, szCaption, MAX_CAPTION);

    PROPSHEETHEADER psh;
    SecureZeroMemory(&psh, sizeof(psh));
    psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_USECALLBACK | PSH_PROPSHEETPAGE;
    psh.hwndParent = g_app.GetMainWnd();
    psh.hInstance = g_hInstance;
    psh.pszCaption = szCaption;
    psh.nPages = MAX_OPTION_PSPS;
    psh.nStartPage = 0;
    psh.ppsp = psps;
    psh.pfnCallback = PropSheetProc;

    PropertySheet(&psh);
}

void COptions::Load()
{
    m_bAutoStart = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_AUTO_START, MOD_AUTO_START, m_szProfilePath);
    GetPrivateProfileString(MOPS_OPTIONS, MOPE_INCLUDE, MOD_INCLUDE_COMMON, m_szInclude, MAX_PATH, m_szProfilePath);
    GetPrivateProfileString(MOPS_OPTIONS, MOPE_EXCLUDE, MOD_EXCLUDE, m_szExclude, MAX_PATH, m_szProfilePath);
    m_bRecurse = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_RECURSE, MOD_RECURSE, m_szProfilePath);

    m_bFolderName = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_FOLDERNAME, MOD_FOLDERNAME, m_szProfilePath);
    GetPrivateProfileString(MOPS_OPTIONS, MOPE_DEFAULT_NAME, MOD_DEFAULT_NAME, m_szDefaultName, MAX_PATH, m_szProfilePath);
    m_iItemDelimiter = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_ITEM_DELIMITER, MOD_ITEM_DELIMITER, m_szProfilePath);
    m_iLineFeed = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_LINE_FEED, MOD_LINE_FEED, m_szProfilePath);
    m_iPathDelimiter = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_PATH_DELIMITER, MOD_PATH_DELIMITER, m_szProfilePath);
    m_bSaveToFullPath = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_SAVE_TO_FULL_PATH, MOD_SAVE_TO_FULL_PATH, m_szProfilePath);

    m_bLowPriority = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_LOW_PRIORITY, MOD_LOW_PRIORITY, m_szProfilePath);
    m_bStoreLFU = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_STORE_LFU, MOD_STORE_LFU, m_szProfilePath);
#ifdef UNICODE
    m_iCodePage = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_CODE_PAGE, MOD_CODE_PAGE, m_szProfilePath);
#endif
    m_bHideAddOptions = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_HIDE_ADD_OPTIONS, MOD_HIDE_ADD_OPTIONS, m_szProfilePath);
    m_bHideSaveOptions = GetPrivateProfileInt(MOPS_OPTIONS, MOPE_HIDE_SAVE_OPTIONS, MOD_HIDE_SAVE_OPTIONS, m_szProfilePath);
}

void COptions::SavePage(int iPage)
{
    switch (iPage)
    {
    case MOPSP_ADD:
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_AUTO_START, m_bAutoStart, m_szProfilePath);
        WritePrivateProfileString(MOPS_OPTIONS, MOPE_INCLUDE, m_szInclude, m_szProfilePath);
        WritePrivateProfileString(MOPS_OPTIONS, MOPE_EXCLUDE, m_szExclude, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_RECURSE, m_bRecurse, m_szProfilePath);
        break;
    case MOPSP_SAVE:
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_FOLDERNAME, m_bFolderName, m_szProfilePath);
        WritePrivateProfileString(MOPS_OPTIONS, MOPE_DEFAULT_NAME, m_szDefaultName, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_ITEM_DELIMITER, m_iItemDelimiter, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_LINE_FEED, m_iLineFeed, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_PATH_DELIMITER, m_iPathDelimiter, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_SAVE_TO_FULL_PATH, m_bSaveToFullPath, m_szProfilePath);
        break;
    case MOPSP_INTEGRATION:
        break;
    case MOPSP_ADVANCED:
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_LOW_PRIORITY, m_bLowPriority, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_STORE_LFU, m_bStoreLFU, m_szProfilePath);
#ifdef UNICODE
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_CODE_PAGE, m_iCodePage, m_szProfilePath);
#endif
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_HIDE_ADD_OPTIONS, m_bHideAddOptions, m_szProfilePath);
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_HIDE_SAVE_OPTIONS, m_bHideSaveOptions, m_szProfilePath);
        break;
    }
}

void COptions::SaveSingle(UINT nID)
{
    switch (nID)
    {
    case IDC_HIDE_ADD_OPTIONS:
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_HIDE_ADD_OPTIONS, m_bHideAddOptions, m_szProfilePath);
        break;
    case IDC_HIDE_SAVE_OPTIONS:
        CProfileHelper::WritePrivateProfileInt(MOPS_OPTIONS, MOPE_HIDE_SAVE_OPTIONS, m_bHideSaveOptions, m_szProfilePath);
        break;
    }
}

void COptions::LoadLastFolderUsed()
{
    if (!GetStoreLFU())
        return;

    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetPrivateProfileString(MOPS_OPTIONS, MOPE_LAST_FOLDER_USED, NULL, szCurrentDir, MAX_PATH, m_szProfilePath);
    SetCurrentDirectory(szCurrentDir);
}

void COptions::SaveLastFolderUsed()
{
    if (!GetStoreLFU())
        return;

    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, szCurrentDir);
    WritePrivateProfileString(MOPS_OPTIONS, MOPE_LAST_FOLDER_USED, szCurrentDir, m_szProfilePath);
}

void COptions::ResetDialog()
{
    CheckDlgButton(m_hWndChildren[MOPSP_ADD], IDC_AUTO_START, MOD_AUTO_START);
    SetDlgItemText(m_hWndChildren[MOPSP_ADD], IDC_INCLUDE, MOD_INCLUDE_COMMON);
    SetDlgItemText(m_hWndChildren[MOPSP_ADD], IDC_EXCLUDE, MOD_EXCLUDE);
    CheckDlgButton(m_hWndChildren[MOPSP_ADD], IDC_RECURSE, MOD_RECURSE);

    CheckRadioButton(m_hWndChildren[MOPSP_SAVE], IDC_FOLDERNAME, IDC_SPECIFY_NAME, MOD_FOLDERNAME ? IDC_FOLDERNAME : IDC_SPECIFY_NAME);
    EnableWindow(GetDlgItem(m_hWndChildren[MOPSP_SAVE], IDC_SPECIFIED_NAME), !MOD_FOLDERNAME);
    SetDlgItemText(m_hWndChildren[MOPSP_SAVE], IDC_SPECIFIED_NAME, MOD_DEFAULT_NAME);
    CheckRadioButton(m_hWndChildren[MOPSP_SAVE], IDC_ID_SPACE_ASTERISK, IDC_ID_VERTICAL_BAR, IDC_ID_SPACE_ASTERISK + MOD_ITEM_DELIMITER);
    CheckRadioButton(m_hWndChildren[MOPSP_SAVE], IDC_LF_CR_LF, IDC_LF_CR, IDC_LF_CR_LF + MOD_LINE_FEED);
    CheckRadioButton(m_hWndChildren[MOPSP_SAVE], IDC_PD_BACKSLASH, IDC_PD_SLASH, IDC_PD_BACKSLASH + MOD_PATH_DELIMITER);
    CheckDlgButton(m_hWndChildren[MOPSP_SAVE], IDC_SAVE_TO_FULL_PATH, MOD_SAVE_TO_FULL_PATH);

    CheckDlgButton(m_hWndChildren[MOPSP_INTEGRATION], IDC_ASSOCIATE, GetAssociated());
    CheckDlgButton(m_hWndChildren[MOPSP_INTEGRATION], IDC_SHORTCUT, IsShortcutAdded());
    CheckDlgButton(m_hWndChildren[MOPSP_INTEGRATION], IDC_INTEGRATE, GetIntegrated());
    CheckDlgButton(m_hWndChildren[MOPSP_INTEGRATION], IDC_SEND_TO, IsSendToAdded());

    CheckDlgButton(m_hWndChildren[MOPSP_ADVANCED], IDC_LOW_PRIORITY, MOD_LOW_PRIORITY);
    CheckDlgButton(m_hWndChildren[MOPSP_ADVANCED], IDC_STORE_LFU, MOD_STORE_LFU);
#ifdef UNICODE
    TCHAR szDefaultCodePageName[MAX_PATH];
    LoadString(g_hInstance, IDS_DEFAULT_CODE_PAGE_NAME, szDefaultCodePageName, MAX_PATH);
    SendMessage(GetDlgItem(m_hWndChildren[MOPSP_ADVANCED], IDC_CODE_PAGE), CB_SELECTSTRING, (WPARAM)-1, (LPARAM)szDefaultCodePageName);
#endif
    CheckDlgButton(m_hWndChildren[MOPSP_ADVANCED], IDC_HIDE_ADD_OPTIONS, MOD_HIDE_ADD_OPTIONS);
    CheckDlgButton(m_hWndChildren[MOPSP_ADVANCED], IDC_HIDE_SAVE_OPTIONS, MOD_HIDE_SAVE_OPTIONS);
}

#ifdef UNICODE
void COptions::ApplyCodePage()
{
    UINT iCodePage = GetCodePage();
    if (iCodePage == CP_OEMCP)
    {
        _tsetlocale(LC_ALL, _T(".OCP"));
    }
    else if ((iCodePage != CP_ACP) && IsValidCodePage(iCodePage))
    {
        TCHAR szCodePage[MAX_PATH];
        _stprintf_s(szCodePage, MAX_PATH, _T(".%d"), iCodePage);
        _tsetlocale(LC_ALL, szCodePage);
    }
    else
    {
        _tsetlocale(LC_ALL, _T(".ACP"));
    }
}
#endif

void COptions::Associate()
{
    TCHAR szText[MAX_TEXT];

    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, MD5_EXTENSION, NULL, REG_SZ, MD5_FILE_TYPE);
    LoadString(g_hInstance, IDS_FILE_TYPE, szText, MAX_TEXT);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, MD5_FILE_TYPE, NULL, REG_SZ, szText);
    _stprintf_s(szText, MAX_TEXT, _T("%s,2"), m_szAppPath);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, MD5_FILE_TYPE _T("\\DefaultIcon"), NULL, REG_SZ, szText);
    _stprintf_s(szText, MAX_TEXT, _T("\"%s\" \"%%1\""), m_szAppPath);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, MD5_FILE_TYPE _T("\\shell\\open\\command"), NULL, REG_SZ, szText);
}

void COptions::Unassociate()
{
    SHDeleteKey(HKEY_CLASSES_ROOT, MD5_EXTENSION);
    SHDeleteKey(HKEY_CLASSES_ROOT, MD5_FILE_TYPE);
}

void COptions::AddShortcut()
{
    TCHAR szShortcutPath[MAX_PATH];
    TCHAR szComment[MAX_TEXT];
    LoadString(g_hInstance, IDS_APP_DESC, szComment, MAX_TEXT);
    if (SHGetSpecialFolderPath(NULL, szShortcutPath, CSIDL_PROGRAMS, FALSE) &&
        PathAppend(szShortcutPath, m_szAppTitle) &&
        (_tcsncat_s(szShortcutPath, MAX_PATH, LINK_EXTENSION, _TRUNCATE) == 0))
        CShellHelper::CreateLink(m_szAppPath, szShortcutPath, szComment);
}

void COptions::RemoveShortcut()
{
    TCHAR szShortcutPath[MAX_PATH];
    if (SHGetSpecialFolderPath(NULL, szShortcutPath, CSIDL_PROGRAMS, FALSE) &&
        PathAppend(szShortcutPath, m_szAppTitle) &&
        (_tcsncat_s(szShortcutPath, MAX_PATH, LINK_EXTENSION, _TRUNCATE) == 0))
        DeleteFile(szShortcutPath);
}

void COptions::Integrate()
{
    TCHAR szText[MAX_TEXT];

    LoadString(g_hInstance, IDS_SHELL_CONTEXT_MENU, szText, MAX_TEXT);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, _T("*\\shell\\") MD5CHECKER, NULL, REG_SZ, szText);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER, NULL, REG_SZ, szText);
    LoadString(g_hInstance, IDS_SCM_CHECK_NEW_ONLY, szText, MAX_TEXT);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER MOPE_CHECK_NEW_ONLY, NULL, REG_SZ, szText);
    _stprintf_s(szText, MAX_TEXT, _T("\"%s\" \"%%1\""), m_szAppPath);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, _T("*\\shell\\") MD5CHECKER _T("\\command"), NULL, REG_SZ, szText);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER _T("\\command"), NULL, REG_SZ, szText);
    _stprintf_s(szText, MAX_TEXT, _T("\"%s\" /%s \"%%1\""), m_szAppPath, MOPE_CHECK_NEW_ONLY);
    CRegistryHelper::SetStringValue(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER MOPE_CHECK_NEW_ONLY _T("\\command"), NULL, REG_SZ, szText);
}

void COptions::Disintegrate()
{
    SHDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\") MD5CHECKER);
    SHDeleteKey(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER);
    SHDeleteKey(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER MOPE_CHECK_NEW_ONLY);
}

void COptions::AddSendTo()
{
    TCHAR szSendToPath[MAX_PATH];
    TCHAR szComment[MAX_TEXT];
    LoadString(g_hInstance, IDS_APP_DESC, szComment, MAX_TEXT);
    if (SHGetSpecialFolderPath(NULL, szSendToPath, CSIDL_SENDTO, FALSE) &&
        PathAppend(szSendToPath, m_szAppTitle) &&
        (_tcsncat_s(szSendToPath, MAX_PATH, LINK_EXTENSION, _TRUNCATE) == 0))
        CShellHelper::CreateLink(m_szAppPath, szSendToPath, szComment);
}

void COptions::RemoveSendTo()
{
    TCHAR szSendToPath[MAX_PATH];
    if (SHGetSpecialFolderPath(NULL, szSendToPath, CSIDL_SENDTO, FALSE) &&
        PathAppend(szSendToPath, m_szAppTitle) &&
        (_tcsncat_s(szSendToPath, MAX_PATH, LINK_EXTENSION, _TRUNCATE) == 0))
        DeleteFile(szSendToPath);
}

void COptions::InitStrings()
{
    GetModuleFileName(g_hInstance, m_szAppPath, MAX_PATH);
    CPathHelper::GetFullPath(m_szAppPath, m_szAppPath);

    CPath profilePath(m_szAppPath);
    profilePath.RemoveExtension();
    profilePath.AddExtension(INI_EXTENSION);
    profilePath.GetString(m_szProfilePath);

    LoadString(g_hInstance, IDS_APP_TITLE, m_szAppTitle, MAX_TEXT);

    LoadString(g_hInstance, IDS_OFN_FOLDER, m_szOFNFolder, MAX_TEXT);
}
