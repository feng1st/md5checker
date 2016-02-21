#include "stdafx.h"
#include "Md5Checker.h"

#include "AboutDialog.h"
#include "AddOptionsDialogStandalone.h"
#include "CalculatorDialog.h"
#include "FileDialog.h"
#include "InternetHelper.h"
#include "MessageBoxHelper.h"
#include "Path.h"
#include "Resource.h"
#include "SaveOptionsDialogStandalone.h"
#include "WaitCursor.h"

#define RELEASE_VERSION     0x0303000C

HINSTANCE g_hInstance = NULL;
CMd5Checker g_app;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    g_hInstance = hInstance;

    if (!g_app.Init())
        return 0;

    return g_app.Run();
}

CMd5Checker::CMd5Checker()
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);

    m_pOptions = new COptions();
    m_pDocument = new CDocument();
    m_pMainWindow = new CMainWindow();
    m_pCheckThread = new CCheckThread();
    m_pAddThread = new CAddThread();
    m_pCompareMD5Dialog = new CCompareMD5Dialog();

#ifdef UNICODE
    m_pOptions->ApplyCodePage();
#endif
}

CMd5Checker::~CMd5Checker()
{
    delete m_pCompareMD5Dialog;
    delete m_pAddThread;
    delete m_pCheckThread;
    delete m_pMainWindow;
    delete m_pDocument;
    delete m_pOptions;

    CoUninitialize();
}

LRESULT CMd5Checker::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_SAVE:
        case ID_ACCEL_FILE_SAVE: // accelerator command, does not correspond to a menu item, no WM_INITMENUPOPUP sent
            OnFileSave();
            break;
        case ID_FILE_SAVE_EACH:
        case ID_ACCEL_FILE_SAVE_EACH:
            OnFileSaveEach();
            break;
        case ID_FILE_SAVE_TO:
            OnFileSaveTo();
            break;
        case ID_FILE_EXIT:
            m_pMainWindow->OnFileExit();
            break;
        case ID_EDIT_ADD:
        case ID_ACCEL_EDIT_ADD:
            OnEditAdd();
            break;
        case ID_EDIT_REMOVE:
        case ID_ACCEL_EDIT_REMOVE:
            m_pMainWindow->GetListView()->OnEditRemove();
            break;
        case ID_EDIT_CLEAR:
        case ID_ACCEL_EDIT_CLEAR:
            OnEditClear();
            break;
        case ID_EDIT_START:
        case ID_ACCEL_EDIT_START:
            OnEditStart();
            break;
        case ID_EDIT_STOP:
        case ID_ACCEL_EDIT_STOP:
            OnEditStop();
            break;
        case ID_EDIT_COMPARE_MD5:
        case ID_ACCEL_EDIT_COMPARE_MD5:
            m_pMainWindow->GetListView()->OnEditCompareMD5();
            break;
        case ID_EDIT_COPY:
        case ID_ACCEL_EDIT_COPY:
            m_pMainWindow->GetListView()->OnEditCopy();
            break;
        case ID_EDIT_COPY_MD5:
            m_pMainWindow->GetListView()->OnEditCopyMD5();
            break;
        case ID_EDIT_COPY_PATHS:
            m_pMainWindow->GetListView()->OnEditCopyPaths();
            break;
        case ID_EDIT_DISCARDED:
        case ID_ACCEL_EDIT_DISCARDED:
            m_pMainWindow->GetListView()->OnEditDiscarded();
            break;
        case ID_EDIT_SELECT_ALL:
        case ID_ACCEL_EDIT_SELECT_ALL:
            m_pMainWindow->GetListView()->OnEditSelectAll();
            break;
        case ID_VIEW_ALL:
        case ID_VIEW_NA:
        case ID_VIEW_UNKNOWN:
        case ID_VIEW_LOADED:
        case ID_VIEW_NEW:
        case ID_VIEW_FAILED:
        case ID_VIEW_PASSED:
            OnView(LOWORD(wParam));
            break;
        case ID_VIEW_LOCATE:
        case ID_ACCEL_VIEW_LOCATE:
            m_pMainWindow->GetListView()->OnViewLocate();
            break;
        case ID_VIEW_PROPERTIES:
        case ID_ACCEL_VIEW_PROPERTIES:
            m_pMainWindow->GetListView()->OnViewProperties();
            break;
        case ID_TOOLS_SELFCHECK:
            OnToolsSelfCheck();
            break;
        case ID_TOOLS_MD5_CALCULATOR:
            OnToolsMD5Calculator();
            break;
        case ID_TOOLS_OPTIONS:
            OnToolsOptions();
            break;
        case ID_ONLINE_HELP:
        case ID_ACCEL_ONLINE_HELP:
            OnOnlineHelp();
            break;
        case ID_HELP_CHECK_FOR_UPDATES:
            OnHelpCheckForUpdates();
            break;
        case ID_HELP_ABOUT:
            OnHelpAbout();
            break;
        }
        break;

    case WM_DESTROY:
        m_pMainWindow->OnDestroy();
        break;

    case WM_DROPFILES:
        OnDropFiles((HDROP)wParam);
        break;

    case WM_INITMENUPOPUP:
        OnInitMenuPopup((HMENU)wParam, (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam));
        break;

    case WM_NOTIFY:
        if (m_pMainWindow->GetListView()->IsNMSender((LPNMHDR)lParam))
        {
            switch (((LPNMHDR)lParam)->code)
            {
            case LVN_COLUMNCLICK:
                OnColumnClick((LPNMLISTVIEW)lParam);
                break;
            case LVN_GETDISPINFO:
                m_pDocument->OnGetDispInfo((LPNMLVDISPINFO)lParam);
                break;
            case LVN_ITEMCHANGED:
                m_pMainWindow->OnItemChanged((LPNMLISTVIEW)lParam);
                break;
            case LVN_ODCACHEHINT:
                m_pDocument->OnODCacheHint((LPNMLVCACHEHINT)lParam);
                break;
            case LVN_ODSTATECHANGED:
                m_pMainWindow->OnODStateChanged((LPNMLVODSTATECHANGE)lParam);
                break;
            case NM_DBLCLK:
                m_pMainWindow->GetListView()->OnEditCompareMD5(TRUE);
                break;
            case NM_RCLICK:
                m_pMainWindow->OnRClick((LPNMITEMACTIVATE)lParam);
                break;
            }
        }
        break;

    case WM_SIZE:
        m_pMainWindow->OnSize((UINT)wParam, (int)LOWORD(lParam), (int)HIWORD(lParam));
        break;

    case WM_SIZING:
        m_pMainWindow->OnSizing((UINT)wParam, (LPRECT)lParam);
        return (LRESULT)TRUE;

    case WM_TIMER:
        switch (wParam)
        {
        case TIMER_ADDING_ID:
            OnTimerAdding();
            break;
        case TIMER_CHECKING_ID:
            OnTimerChecking();
            break;
        }
        break;

    case WM_USER_ADD_CANCELLED:
        OnAddCancelled();
        break;

    case WM_USER_ADD_STARTED:
        OnAddStarted();
        break;

    case WM_USER_ADD_STOPPED:
        OnAddStopped();
        break;

    case WM_USER_CHECK_CANCELLED:
        OnCheckCancelled();
        break;

    case WM_USER_CHECK_STARTED:
        OnCheckStarted();
        break;

    case WM_USER_CHECK_STOPPED:
        OnCheckStopped();
        break;

    case WM_USER_MD5_ITEMS_DELETED:
        OnMd5ItemsDeleted();
        break;

    case WM_USER_PROGRESS_BAR_SHOW:
        m_pMainWindow->OnProgressBarShow((BOOL)wParam);
        break;

    case WM_USER_SELECTION_CHANGED:
        m_pMainWindow->OnSelectionChanged();
        break;

    case WM_USER_UPDATE_VIEW_TOOLBAR:
        m_pMainWindow->OnUpdateViewToolbar();
        break;

    case WM_USER_SET_CURRENT_MD5:
        m_pDocument->SetMd5ItemCurrentMD5((LPCTSTR)wParam, (LPBYTE)lParam);
        break;

    case WM_USER_MARK_AS_NA:
        return (LRESULT)m_pDocument->MarkMd5ItemAsNA((LPCTSTR)wParam);

    case WM_USER_UPDATE_TITLE:
        m_pMainWindow->OnUpdateTitle((LPCTSTR)wParam);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return (LRESULT)FALSE;
}

BOOL CMd5Checker::Init()
{
    if (m_pMainWindow->Create() == NULL)
        return FALSE;

    BOOL rbMixed;
    ParseCommandLine(rbMixed);

    // Adding paths specified in command line parameters.
    m_pDocument->AcquireReadPathQueue();
    BOOL bHasPathQueued = m_pDocument->HasPathQueued();
    m_pDocument->ReleaseReadPathQueue();
    if (bHasPathQueued && (CAddOptionsDialogStandalone().Show(rbMixed ? 0 : CAddOptionsDialogStandalone::DisableAddFolder) == IDOK))
        m_pAddThread->Start();

    return TRUE;
}

void CMd5Checker::ParseCommandLine(BOOL &rbMixed)
{
    TCHAR szPath[MAX_PATH];
    rbMixed = FALSE;
    m_pDocument->AcquireWritePathQueue();
    for (int iArg = 1; iArg < __argc; iArg++)
    {
		if ((__targv[iArg][0] == _T('-')) || (__targv[iArg][0] == _T('/')))
		{
            if (_tcsicmp(&__targv[iArg][1], MOPE_CHECK_NEW_ONLY) == 0)
            {
                m_pOptions->SetFlag(COptions::OnceCheck, COptions::OnceCheck);
                m_pOptions->SetCheckNewOnly(TRUE);
                continue;
            }
        }

        CPathHelper::Cleanup(__targv[iArg], szPath, _T("*?\"<>|\r\n"));
        if (PathFileExists(szPath))
        {
            if (!rbMixed && PathIsDirectory(szPath))
                rbMixed = TRUE;

            m_pDocument->QueuePath(szPath);
        }
    }
    m_pDocument->ReleaseWritePathQueue();
}

int CMd5Checker::Run()
{
    MSG msg;
    HACCEL hAccelTable;

    if ((hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_MD5CHECKER))) == NULL)
        return 0;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!m_pCompareMD5Dialog->IsDialogMessage(&msg) &&
            !TranslateAccelerator(GetMainWnd(), hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

void CMd5Checker::OnFileSave()
{
    m_pDocument->AcquireRead();
    if (m_pDocument->GetList()->GetSize() == 0)
    {
        m_pDocument->ReleaseRead();
        return;
    }
    m_pDocument->ReleaseRead();

    m_pAddThread->Stop();
    m_pCheckThread->Stop();

    if (CSaveOptionsDialogStandalone().Show(0) != IDOK)
        return;

    CWaitCursor waitCursor;

    m_pDocument->Save();
    m_pDocument->ClearDiscarded();

    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();

    m_pOptions->SetFlag(COptions::OnceSave, 0);
}

void CMd5Checker::OnFileSaveEach()
{
    m_pDocument->AcquireRead();
    if (m_pDocument->GetList()->GetSize() == 0)
    {
        m_pDocument->ReleaseRead();
        return;
    }
    m_pDocument->ReleaseRead();

    m_pAddThread->Stop();
    m_pCheckThread->Stop();

    if (CSaveOptionsDialogStandalone().Show(CSaveOptionsDialogStandalone::DisableDefaultFileName | CSaveOptionsDialogStandalone::DisablePathDelimiter) != IDOK)
        return;

    CWaitCursor waitCursor;

    m_pDocument->SaveEach();
    m_pDocument->ClearDiscarded();

    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();

    m_pOptions->SetFlag(COptions::OnceSave, 0);
}

void CMd5Checker::OnFileSaveTo()
{
    m_pDocument->AcquireRead();
    if (m_pDocument->GetList()->GetSize() == 0)
    {
        m_pDocument->ReleaseRead();
        return;
    }
    m_pDocument->ReleaseRead();

    m_pAddThread->Stop();
    m_pCheckThread->Stop();

    CWaitCursor waitCursor;

    TCHAR szFileName[MAX_PATH] = { 0 };
    CPathHelper::Cleanup(m_pOptions->GetDefaultName(TRUE), szFileName);
    if (*szFileName == 0)
        _tcsncpy_s(szFileName, MAX_PATH, MOD_DEFAULT_NAME, _TRUNCATE);
    _tcsncat_s(szFileName, MAX_PATH, MD5_EXTENSION, _TRUNCATE);

    CFileDialog saveDialog(FALSE, szFileName, MAX_PATH,
        OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_EXPLORER | OFN_OVERWRITEPROMPT,
        IDS_FILE_FILTER, 2, CFileDialog::SaveToDialogProc, IDD_SAVE_OPTIONS_EMBEDDED, GetMainWnd());
    if (saveDialog.Show() != NULL)
    {
        m_pDocument->Save(szFileName);
        m_pDocument->ClearDiscarded();

        m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();

        m_pOptions->SetFlag(COptions::OnceSave, 0);
    }
}

void CMd5Checker::OnEditAdd()
{
    TCHAR szPaths[MAX_PATHS] = { 0 };
    CFileDialog openDialog(TRUE, szPaths, MAX_PATHS,
        OFN_ALLOWMULTISELECT | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOVALIDATE,
        IDS_FILE_FILTER, 1, CFileDialog::AddDialogProc, IDD_ADD_OPTIONS_EMBEDDED, GetMainWnd());

    if (openDialog.Show() != NULL)
    {
        m_pDocument->ParsePaths(szPaths, TRUE);
        m_pAddThread->Start();
    }
}

void CMd5Checker::OnEditClear()
{
    m_pAddThread->Stop();
    m_pCheckThread->Stop();

    SendMessage(WM_USER_UPDATE_TITLE, NULL, 0);
    m_pMainWindow->GetListView()->SetItemCountEx(0, LVSICF_NOSCROLL);
    m_pDocument->Clear();
    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();
    m_pMainWindow->UpdateToolbar();
    m_pMainWindow->UpdateStatusBar();
}

void CMd5Checker::OnEditStart()
{
    if ((GetAsyncKeyState(VK_SHIFT) & ~1) != 0)
    {
        m_pOptions->SetFlag(COptions::OnceCheck, COptions::OnceCheck);
        m_pOptions->SetCheckNewOnly(TRUE);
    }

    m_pCheckThread->Start();
}

void CMd5Checker::OnEditStop()
{
    if (m_pAddThread->IsAdding())
    {
        if (CMessageBoxHelper::Show(g_hInstance, GetMainWnd(), IDS_ABORT_ADDING, IDS_ABORT, MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            m_pAddThread->Stop();

            if (m_pCheckThread->IsChecking() && !m_pOptions->GetAutoStart())
                m_pCheckThread->Stop();
        }
    }
    else if (m_pCheckThread->IsChecking())
    {
        m_pCheckThread->Stop();
    }
}

void CMd5Checker::OnView(UINT uID)
{
    CheckMenuRadioItem(GetMenu(m_pMainWindow->GetWnd()), ID_VIEW_ALL, ID_VIEW_PASSED, uID, MF_BYCOMMAND);
    m_pMainWindow->GetReBar()->GetViewToolbar()->CheckButton(uID);

    m_pDocument->AcquireWrite();

    int nCount = m_pDocument->SetCategory(uID - ID_VIEW_ALL);
    if (nCount != -1)
    {
        m_pMainWindow->GetListView()->DeleteAllItems();
        m_pMainWindow->GetListView()->SetItemCountEx(nCount, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
        m_pMainWindow->UpdateToolbar();
        m_pMainWindow->UpdateStatusBar();
    }

    m_pDocument->ReleaseWrite();
}

void CMd5Checker::OnToolsSelfCheck()
{
    LPCTSTR lpszAppPath = m_pOptions->GetAppPath();
    CPath folder(lpszAppPath);
    if (!folder.RemoveFileSpec())
        return;

    if (m_pDocument->LoadAddFile(folder.GetPath(), lpszAppPath))
        SendMessage(WM_USER_ADD_STOPPED, 0, 0);
}

void CMd5Checker::OnToolsMD5Calculator()
{
    CCalculatorDialog calculatorDialog;
    calculatorDialog.Show();
}

void CMd5Checker::OnToolsOptions()
{
    m_pOptions->Show();
}

void CMd5Checker::OnOnlineHelp()
{
    TCHAR szHelpURL[MAX_PATH];
    LoadString(g_hInstance, IDS_HELP_URL, szHelpURL, MAX_PATH);
    ShellExecute(GetMainWnd(), _T("open"), szHelpURL, NULL, NULL, SW_SHOW);
}

void CMd5Checker::OnHelpCheckForUpdates()
{
    CHAR szVersionHex[MAX_PATH];
    TCHAR szVersionURL[MAX_PATH];
    TCHAR szDownloadURL[MAX_PATH];
    LoadString(g_hInstance, IDS_VERSION_URL, szVersionURL, MAX_PATH);
    LoadString(g_hInstance, IDS_DOWNLOAD_URL, szDownloadURL, MAX_PATH);
    int versionLatest;
    if (!CInternetHelper::ReadA(szVersionURL, NULL, szVersionHex, MAX_PATH) ||
        (szVersionHex[0] != '0') || ((szVersionHex[1] != 'x') && (szVersionHex[1] != 'X')) ||
        (sscanf_s(szVersionHex, "%x", &versionLatest) != 1))
        CMessageBoxHelper::Show(g_hInstance, GetMainWnd(), IDS_CFU_ERROR, IDS_CHECK_FOR_UPDATES, MB_ICONINFORMATION | MB_OK);
    else if (versionLatest <= RELEASE_VERSION)
        CMessageBoxHelper::Show(g_hInstance, GetMainWnd(), IDS_CFU_NA, IDS_CHECK_FOR_UPDATES, MB_ICONINFORMATION | MB_OK);
    else if (CMessageBoxHelper::Show(g_hInstance, GetMainWnd(), IDS_CFU_AVAILABLE, IDS_CHECK_FOR_UPDATES, MB_ICONQUESTION | MB_YESNO) == IDYES)
        ShellExecute(GetMainWnd(), _T("open"), szDownloadURL, NULL, NULL, SW_SHOW);
}

void CMd5Checker::OnHelpAbout()
{
    CAboutDialog aboutDialog;
    aboutDialog.Show();
}

void CMd5Checker::OnDropFiles(HDROP hDrop)
{
    UINT nFiles = DragQueryFile(hDrop, (UINT)-1, NULL, 0);

    BOOL bMixed = FALSE;
    TCHAR szPath[MAX_PATH];
    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    m_pDocument->AcquireWritePathQueue();
    for (UINT iFile = 0; iFile < nFiles; iFile++)
    {
        DragQueryFile(hDrop, iFile, szPath, MAX_PATH);

        if (PathFileExists(szPath))
        {
            if (*szCurrentDir == 0)
                _tcscpy_s(szCurrentDir, MAX_PATH, szPath);

            if (!bMixed && PathIsDirectory(szPath))
                bMixed = TRUE;

            m_pDocument->QueuePath(szPath);
        }
    }
    m_pDocument->ReleaseWritePathQueue();

    if (*szCurrentDir != 0)
    {
        if (!PathIsDirectory(szCurrentDir))
            PathRemoveFileSpec(szCurrentDir);
        SetCurrentDirectory(szCurrentDir);
    }

    m_pDocument->AcquireReadPathQueue();
    BOOL bHasPathQueued = m_pDocument->HasPathQueued();
    m_pDocument->ReleaseReadPathQueue();
    if (bHasPathQueued && (CAddOptionsDialogStandalone().Show(bMixed ? 0 : CAddOptionsDialogStandalone::DisableAddFolder) == IDOK))
        m_pAddThread->Start();

    DragFinish(hDrop);
}

void CMd5Checker::OnInitMenuPopup(HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    UNREFERENCED_PARAMETER(nIndex);

    if (bSysMenu)
        return;

    int nMenuItems = GetMenuItemCount(hPopupMenu);
    int i;
    UINT uID;

    CListView *pListView = m_pMainWindow->GetListView();
    int nSelected = pListView->GetSelectedCount();
    CMd5Item *pMd5Item;

    for (i = 0; i < nMenuItems; i++)
    {
        uID = GetMenuItemID(hPopupMenu, i);

        switch (uID)
        {
        case ID_EDIT_REMOVE:
        case ID_EDIT_COPY:
        case ID_EDIT_COPY_PATHS:
            if (nSelected > 0)
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_ENABLED);
            else
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_GRAYED);
            break;

        case ID_EDIT_COMPARE_MD5:
            if ((nSelected == 1) || (nSelected == 2))
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_ENABLED);
            else
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_GRAYED);
            break;

        case ID_EDIT_DISCARDED:
            if (nSelected > 0)
            {
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_ENABLED);
                m_pDocument->AcquireRead();
                pMd5Item = m_pDocument->GetCache()->GetMd5Item(pListView->GetSelectionMark(), m_pDocument->GetCount(), m_pDocument->GetCategory());
                if ((pMd5Item == NULL) || !pMd5Item->IsDiscarded())
                    CheckMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_UNCHECKED);
                else
                    CheckMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_CHECKED);
                m_pDocument->ReleaseRead();
            }
            else
            {
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_GRAYED);
            }
            break;

        case ID_EDIT_COPY_MD5:
        case ID_VIEW_LOCATE:
        case ID_VIEW_PROPERTIES:
            if (nSelected == 1)
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_ENABLED);
            else
                EnableMenuItem(hPopupMenu, uID, MF_BYCOMMAND | MF_GRAYED);
            break;
        }
    }
}

void CMd5Checker::OnColumnClick(LPNMLISTVIEW lpnm)
{
    if (m_pDocument->GetCount() == 0)
        return;

    CWaitCursor waitCursor;

    int iSort = lpnm->iSubItem;
    if (iSort == m_iSort)
    {
        m_bAsc = !m_bAsc;
    }
    else
    {
        m_iSort = iSort;
        m_bAsc = TRUE;
    }

    m_pDocument->AcquireWrite();

    m_pDocument->Sort(m_iSort, m_bAsc);

    m_pDocument->GetCache()->Clear();
    m_pMainWindow->GetListView()->SetItemState(-1, 0, LVIS_SELECTED);
    m_pMainWindow->GetListView()->SetItemCountEx(m_pDocument->GetCount(), LVSICF_NOSCROLL);

    m_pDocument->ReleaseWrite();

    m_pMainWindow->UpdateToolbar();
    m_pMainWindow->UpdateStatusBar();
}

void CMd5Checker::OnTimerAdding()
{
    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();
    m_pMainWindow->GetListView()->SetItemCountEx(m_pDocument->GetCount(), LVSICF_NOSCROLL);
    if (!m_pCheckThread->IsChecking())
        m_pMainWindow->GetStatusBar()->SetTextAdding(m_pAddThread->GetCurrentPath());
    m_pAddThread->CheckStop();
}

void CMd5Checker::OnTimerChecking()
{
    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();
    m_pMainWindow->GetStatusBar()->SetTextChecking(
        m_pDocument->GetCount(MIC_ALL) - m_pDocument->GetCount(MIC_UNKNOWN) - m_pDocument->GetCount(MIC_LOADED),
        m_pDocument->GetCount(MIC_ALL), m_pCheckThread->GetCurrentFilePath());
    m_pMainWindow->GetStatusBar()->GetProgressBar()->SetPostion(m_pCheckThread->GetFileCompletedPercent());
    m_pCheckThread->CheckStop();
}

void CMd5Checker::OnAddCancelled()
{
    m_pOptions->SetFlag(COptions::OnceAdd, 0);
}

void CMd5Checker::OnAddStarted()
{
    m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_START, TRUE);
    m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_STOP, FALSE);
}

void CMd5Checker::OnAddStopped()
{
    if (!m_pCheckThread->IsChecking())
    {
        m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_START, FALSE);
        m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_STOP, TRUE);
    }
    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();
    m_pMainWindow->GetListView()->SetItemCountEx(m_pDocument->GetCount(), 0);
    m_pMainWindow->UpdateToolbar();
    m_pMainWindow->UpdateStatusBar(TRUE);

    if (m_pOptions->GetAutoStart()) // still using last add options here, i.e. DO NOT change add options flag
        m_pCheckThread->Start();

    m_pOptions->SetFlag(COptions::OnceAdd, 0); // can change add options flag now
}

void CMd5Checker::OnCheckCancelled()
{
    m_pOptions->SetFlag(COptions::OnceCheck, 0);
}

void CMd5Checker::OnCheckStarted()
{
    m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_START, TRUE);
    m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_STOP, FALSE);
    m_pMainWindow->GetStatusBar()->GetProgressBar()->Show(TRUE);
}

void CMd5Checker::OnCheckStopped()
{
    if (!m_pAddThread->IsAdding())
    {
        m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_START, FALSE);
        m_pMainWindow->GetReBar()->GetMainToolbar()->HideButton(ID_EDIT_STOP, TRUE);
    }
    m_pMainWindow->GetStatusBar()->GetProgressBar()->Show(FALSE);
    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();
    m_pMainWindow->GetListView()->SetItemCountEx(m_pDocument->GetCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
    m_pMainWindow->UpdateToolbar();
    m_pMainWindow->UpdateStatusBar(TRUE);

    m_pOptions->SetFlag(COptions::OnceCheck, 0);
}

void CMd5Checker::OnMd5ItemsDeleted()
{
    m_pMainWindow->GetReBar()->GetViewToolbar()->UpdateTexts();
    m_pCheckThread->CheckDeleted();
}
