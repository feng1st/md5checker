#pragma once

#include "CommonDefine.h"
#include "RegistryHelper.h"

#define MD5CHECKER                  _T("Md5Checker")
#define MD5_EXTENSION               _T(".md5")
#define MD5_FILE_TYPE               _T("md5file")

#define INI_EXTENSION               _T(".ini")
#define LINK_EXTENSION              _T(".lnk")

// Md5 Option Enumeration
#define MOE_ID_SPACE_ASTERISK       0
#define MOE_ID_SPACES               1
#define MOE_ID_VERTICAL_BAR         2

#define MOE_LF_CR_LF                0
#define MOE_LF_LF                   1
#define MOE_LF_CR                   2

#define MOE_PD_BACKSLASH            0
#define MOE_PD_SLASH                1

// Md5 Option Default Values
#define MOD_UI                      TRUE
#define MOD_AUTO_START              TRUE
#define MOD_CHECK_NEW_ONLY          FALSE
#define MOD_INCLUDE_COMMON          _T("*.dll;*.drv;*.exe;*.msi;*.ocx;*.sys;*.zip;*.rar;*.iso")
#define MOD_INCLUDE_EXE             _T("*.dll;*.drv;*.exe;*.msi;*.ocx;*.sys")
#define MOD_INCLUDE_ALL             _T("*")
#define MOD_EXCLUDE                 _T("")
#define MOD_RECURSE                 TRUE
#define MOD_FOLDERNAME              FALSE
#define MOD_DEFAULT_NAME            _T("CHECKSUM")
#define MOD_ITEM_DELIMITER          MOE_ID_SPACE_ASTERISK
#define MOD_LINE_FEED               MOE_LF_CR_LF
#define MOD_PATH_DELIMITER          MOE_PD_BACKSLASH
#define MOD_SAVE_TO_FULL_PATH       FALSE
#define MOD_LOW_PRIORITY            FALSE
#define MOD_STORE_LFU               FALSE
#ifdef UNICODE
#define MOD_CODE_PAGE               CP_ACP
#endif
#define MOD_HIDE_ADD_OPTIONS        TRUE
#define MOD_HIDE_SAVE_OPTIONS       TRUE

// Md5 Option Profile Sections
#define MOPS_OPTIONS                _T("Options")

// Md5 Option Profile Entries
#define MOPE_AUTO_START             _T("AutoStart")
#define MOPE_CHECK_NEW_ONLY         _T("CheckNewOnly")
#define MOPE_INCLUDE                _T("Include")
#define MOPE_EXCLUDE                _T("Exclude")
#define MOPE_RECURSE                _T("Recurse")
#define MOPE_FOLDERNAME             _T("FolderName")
#define MOPE_DEFAULT_NAME           _T("DefaultName")
#define MOPE_ITEM_DELIMITER         _T("ItemDelimiter")
#define MOPE_LINE_FEED              _T("LineFeed")
#define MOPE_PATH_DELIMITER         _T("PathDelimiter")
#define MOPE_SAVE_TO_FULL_PATH      _T("SaveToFullPath")
#define MOPE_LOW_PRIORITY           _T("LowPriority")
#define MOPE_STORE_LFU              _T("StoreLastFolderUsed")
#ifdef UNICODE
#define MOPE_CODE_PAGE              _T("CodePage")
#endif
#define MOPE_HIDE_ADD_OPTIONS       _T("HideAddOptions")
#define MOPE_HIDE_SAVE_OPTIONS      _T("HideSaveOptions")
#define MOPE_LAST_FOLDER_USED       _T("LastFolderUsed")

#define MAX_OPTION_PSPS             4
#define MOPSP_ADD                   0
#define MOPSP_SAVE                  1
#define MOPSP_INTEGRATION           2
#define MOPSP_ADVANCED              3

class COptions
{
public:
    static const int OnceAdd    = 0x1;
    static const int OnceCheck  = 0x2;
    static const int OnceSave   = 0x4;

public:
    static int CALLBACK PropSheetProc(HWND hwndDlg, UINT uMsg, LPARAM lParam);

public:
    COptions();
    virtual ~COptions();

public:

    virtual void SetChildWnd(int i, HWND hWnd)
    {
        m_hWndChildren[i] = hWnd;
    }

    virtual void SetFlag(int nMask, int nFlag)
    {
        m_nFlag = (m_nFlag & ~nMask) | (nFlag & nMask);
    }

    virtual BOOL GetAutoStart(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceAdd) == 0))
            return m_bAutoStart;
        else
            return m_bAutoStartOnce;
    }

    virtual void SetAutoStart(BOOL bAutoStart, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceAdd) == 0))
            m_bAutoStart = bAutoStart;
        else
            m_bAutoStartOnce = bAutoStart;
    }

    virtual BOOL GetCheckNewOnly(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceCheck) == 0))
            return MOD_CHECK_NEW_ONLY;
        else
            return m_bCheckNewOnlyOnce;
    }

    virtual void SetCheckNewOnly(BOOL bCheckNewOnly)
    {
        if ((m_nFlag & OnceCheck) == OnceCheck)
            m_bCheckNewOnlyOnce = bCheckNewOnly;
    }

    virtual LPCTSTR GetInclude(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceAdd) == 0))
            return m_szInclude;
        else
            return m_szIncludeOnce;
    }

    virtual LPCTSTR GetExclude(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceAdd) == 0))
            return m_szExclude;
        else
            return m_szExcludeOnce;
    }

    virtual BOOL GetRecurse(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceAdd) == 0))
            return m_bRecurse;
        else
            return m_bRecurseOnce;
    }

    virtual void SetRecurse(BOOL bRecurse, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceAdd) == 0))
            m_bRecurse = bRecurse;
        else
            m_bRecurseOnce = bRecurse;
    }

    virtual BOOL GetFolderName(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            return m_bFolderName;
        else
            return m_bFolderNameOnce;
    }

    virtual void SetFolderName(BOOL bFolderName, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            m_bFolderName = bFolderName;
        else
            m_bFolderNameOnce = bFolderName;
    }

    virtual LPCTSTR GetDefaultName(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            return m_szDefaultName;
        else
            return m_szDefaultNameOnce;
    }

    virtual int GetItemDelimiter(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            return m_iItemDelimiter;
        else
            return m_iItemDelimiterOnce;
    }

    virtual void SetItemDelimiter(int iItemDelimiter, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            m_iItemDelimiter = iItemDelimiter;
        else
            m_iItemDelimiterOnce = iItemDelimiter;
    }

    virtual int GetLineFeed(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            return m_iLineFeed;
        else
            return m_iLineFeedOnce;
    }

    virtual void SetLineFeed(int iLineFeed, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            m_iLineFeed = iLineFeed;
        else
            m_iLineFeedOnce = iLineFeed;
    }

    virtual int GetPathDelimiter(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            return m_iPathDelimiter;
        else
            return m_iPathDelimiterOnce;
    }

    virtual void SetPathDelimiter(int iPathDelimiter, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            m_iPathDelimiter = iPathDelimiter;
        else
            m_iPathDelimiterOnce = iPathDelimiter;
    }

    virtual BOOL GetSaveToFullPath(BOOL bDefault = FALSE) const
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            return m_bSaveToFullPath;
        else
            return m_bSaveToFullPathOnce;
    }

    virtual void SetSaveToFullPath(BOOL bSaveToFullPath, BOOL bDefault = FALSE)
    {
        if (bDefault || ((m_nFlag & OnceSave) == 0))
            m_bSaveToFullPath = bSaveToFullPath;
        else
            m_bSaveToFullPathOnce = bSaveToFullPath;
    }

    virtual BOOL GetLowPriority() const
    {
        return m_bLowPriority;
    }

    virtual void SetLowPriority(BOOL bLowPriority)
    {
        m_bLowPriority = bLowPriority;
    }

    virtual BOOL GetStoreLFU() const
    {
        return m_bStoreLFU;
    }

    virtual void SetStoreLFU(BOOL bStoreLFU)
    {
        m_bStoreLFU = bStoreLFU;
    }

#ifdef UNICODE
    virtual UINT GetCodePage() const
    {
        return m_iCodePage;
    }

    virtual void SetCodePage(UINT iCodePage)
    {
        if (iCodePage != m_iCodePage)
        {
            m_iCodePage = iCodePage;
            ApplyCodePage();
        }
    }
#endif

    virtual BOOL GetHideAddOptions() const
    {
        return m_bHideAddOptions;
    }

    virtual void SetHideAddOptions(BOOL bHideAddOptions)
    {
        m_bHideAddOptions = bHideAddOptions;
    }

    virtual BOOL GetHideSaveOptions() const
    {
        return m_bHideSaveOptions;
    }

    virtual void SetHideSaveOptions(BOOL bHideSaveOptions)
    {
        m_bHideSaveOptions = bHideSaveOptions;
    }

    virtual BOOL GetAssociated() const
    {
        TCHAR szText[MAX_TEXT];
        DWORD dwText = MAX_TEXT;
        return (CRegistryHelper::QueryValue(HKEY_CLASSES_ROOT, MD5_EXTENSION, NULL, (LPBYTE)szText, &dwText) && (_tcsicmp(szText, MD5_FILE_TYPE) == 0));
    }

    virtual BOOL IsShortcutAdded() const
    {
        TCHAR szShortcutPath[MAX_PATH];
        return SHGetSpecialFolderPath(NULL, szShortcutPath, CSIDL_PROGRAMS, FALSE) &&
            PathAppend(szShortcutPath, m_szAppTitle) &&
            (_tcsncat_s(szShortcutPath, MAX_PATH, LINK_EXTENSION, _TRUNCATE) == 0) &&
            PathFileExists(szShortcutPath);
    }

    virtual BOOL GetIntegrated() const
    {
        return CRegistryHelper::QueryValue(HKEY_CLASSES_ROOT, _T("*\\shell\\") MD5CHECKER, NULL, NULL, NULL) &&
            CRegistryHelper::QueryValue(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER, NULL, NULL, NULL) &&
            CRegistryHelper::QueryValue(HKEY_CLASSES_ROOT, _T("Folder\\shell\\") MD5CHECKER MOPE_CHECK_NEW_ONLY, NULL, NULL, NULL);
    }

    virtual BOOL IsSendToAdded() const
    {
        TCHAR szSendToPath[MAX_PATH];
        return SHGetSpecialFolderPath(NULL, szSendToPath, CSIDL_SENDTO, FALSE) &&
            PathAppend(szSendToPath, m_szAppTitle) &&
            (_tcsncat_s(szSendToPath, MAX_PATH, LINK_EXTENSION, _TRUNCATE) == 0) &&
            PathFileExists(szSendToPath);
    }

    virtual LPCTSTR GetAppTitle() const
    {
        return m_szAppTitle;
    }

    virtual LPCTSTR GetAppPath() const
    {
        return m_szAppPath;
    }

    virtual LPCTSTR GetOFNFolder() const
    {
        return m_szOFNFolder;
    }

public:
    virtual void Show();
    virtual void Load();
    virtual void SavePage(int iPage);
    virtual void SaveSingle(UINT nID);
    virtual void LoadLastFolderUsed();
    virtual void SaveLastFolderUsed();
    virtual void ResetDialog();
#ifdef UNICODE
    virtual void ApplyCodePage();
#endif
    virtual void Associate();
    virtual void Unassociate();
    virtual void AddShortcut();
    virtual void RemoveShortcut();
    virtual void Integrate();
    virtual void Disintegrate();
    virtual void AddSendTo();
    virtual void RemoveSendTo();

protected:
    virtual void InitStrings();

protected:
    int m_nFlag;

    BOOL m_bAutoStart;
    TCHAR m_szInclude[MAX_PATH];
    TCHAR m_szExclude[MAX_PATH];
    BOOL m_bRecurse;
    BOOL m_bFolderName;
    TCHAR m_szDefaultName[MAX_PATH];
    int m_iItemDelimiter;
    int m_iLineFeed;
    int m_iPathDelimiter;
    BOOL m_bSaveToFullPath;
    BOOL m_bLowPriority;
    BOOL m_bStoreLFU;
#ifdef UNICODE
    UINT m_iCodePage;
#endif
    BOOL m_bHideAddOptions;
    BOOL m_bHideSaveOptions;

    BOOL m_bAutoStartOnce;
    BOOL m_bCheckNewOnlyOnce;
    TCHAR m_szIncludeOnce[MAX_PATH];
    TCHAR m_szExcludeOnce[MAX_PATH];
    BOOL m_bRecurseOnce;
    BOOL m_bFolderNameOnce;
    TCHAR m_szDefaultNameOnce[MAX_PATH];
    int m_iItemDelimiterOnce;
    int m_iLineFeedOnce;
    int m_iPathDelimiterOnce;
    BOOL m_bSaveToFullPathOnce;

    HWND m_hWndChildren[MAX_OPTION_PSPS];

    TCHAR m_szAppTitle[MAX_CAPTION];

    TCHAR m_szAppPath[MAX_PATH];
    TCHAR m_szProfilePath[MAX_PATH];

    TCHAR m_szOFNFolder[MAX_TEXT];
};
