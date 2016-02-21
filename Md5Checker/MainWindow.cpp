#include "stdafx.h"
#include "MainWindow.h"

#include "Md5Checker.h"
#include "Options.h"
#include "Resource.h"

#define MIN_MAIN_WINDOW_WIDTH       600
#define MIN_MAIN_WINDOW_HEIGHT      450

LRESULT CALLBACK CMainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return g_app.WindowProc(hwnd, uMsg, wParam, lParam);
}

CMainWindow::CMainWindow()
{
    m_pReBar = NULL;
    m_pListView = NULL;
    m_pStatusBar = NULL;
}

CMainWindow::~CMainWindow()
{
    if (m_pReBar != NULL)
    {
        delete m_pReBar;
        m_pReBar = NULL;
    }
    if (m_pListView != NULL)
    {
        delete m_pListView;
        m_pListView = NULL;
    }
    if (m_pStatusBar != NULL)
    {
        delete m_pStatusBar;
        m_pStatusBar = NULL;
    }
}

HWND CMainWindow::Create()
{
    TCHAR szClassName[MAX_CAPTION];

    if (LoadString(g_hInstance, IDC_MD5CHECKER, szClassName, MAX_CAPTION) == 0)
        return NULL;

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(wcex);
    wcex.style = 0;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInstance;
    wcex.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MD5CHECKER));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MD5CHECKER);
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_SMALL));
    if (RegisterClassEx(&wcex) == 0)
        return NULL;

    if ((m_hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, szClassName, g_app.GetOptions()->GetAppTitle(), WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, MIN_MAIN_WINDOW_WIDTH, MIN_MAIN_WINDOW_HEIGHT, NULL, NULL, g_hInstance, NULL)) == NULL)
        return NULL;

    if (((m_pReBar = new CReBar(m_hWnd)) == NULL) || (m_pReBar->Create() == NULL))
        return NULL;

    if (((m_pListView = new CListView(m_hWnd)) == NULL) || (m_pListView->Create() == NULL))
        return NULL;

    if (((m_pStatusBar = new CStatusBar(m_hWnd)) == NULL) || (m_pStatusBar->Create() == NULL))
        return NULL;

    DragAcceptFiles(m_hWnd, TRUE);
    CheckMenuRadioItem(GetMenu(m_hWnd), ID_VIEW_ALL, ID_VIEW_PASSED, ID_VIEW_ALL, MF_BYCOMMAND);
    UpdateToolbar(TRUE);
    UpdateStatusBar(TRUE);

    ShowWindow(m_hWnd, SW_SHOW);

    if (!UpdateWindow(m_hWnd))
        return NULL;

    return m_hWnd;
}

void CMainWindow::UpdateToolbar(BOOL bForce)
{
    static int nSelectedLast;

    int nSelected = m_pListView->GetSelectedCount();

    if (bForce || (nSelected != nSelectedLast))
        m_pReBar->GetMainToolbar()->EnableButtons(nSelected);

    nSelectedLast = nSelected;
}

void CMainWindow::UpdateStatusBar(BOOL bForce)
{
    static int nCountLast, nSelectedLast;

    int nCount = m_pListView->GetItemCount();
    int nSelected = m_pListView->GetSelectedCount();

    if ((nSelected == 0) && (bForce || (nSelected != nSelectedLast) || (nCount != nCountLast)))
        m_pStatusBar->SetTextUnselected(nCount);
    else if ((nSelected > 0) && (bForce || (nSelected != nSelectedLast)))
        m_pStatusBar->SetTextSelected(nSelected);

    nCountLast = nCount;
    nSelectedLast = nSelected;
}

void CMainWindow::OnFileExit()
{
    DestroyWindow(m_hWnd);
}

void CMainWindow::OnDestroy()
{
    PostQuitMessage(0);
}

void CMainWindow::OnItemChanged(LPNMLISTVIEW lpnm)
{
    if (((lpnm->uChanged & LVIF_STATE) == LVIF_STATE) && (((lpnm->uNewState ^ lpnm->uOldState) & LVIS_SELECTED) == LVIS_SELECTED))
        // POST
        PostMessage(WM_USER_SELECTION_CHANGED, 0, 0);
}

void CMainWindow::OnODStateChanged(LPNMLVODSTATECHANGE lpnm)
{
    UNREFERENCED_PARAMETER(lpnm);

    UpdateToolbar();
    UpdateStatusBar();
}

void CMainWindow::OnRClick(LPNMITEMACTIVATE lpnm)
{
    UNREFERENCED_PARAMETER(lpnm);

    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENUS));
    HMENU hSubMenu = GetSubMenu(hMenu, 0);

    SetMenuDefaultItem(hSubMenu, ID_EDIT_COMPARE_MD5, FALSE);
    TrackPopupMenuEx(hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, m_hWnd, NULL);

    DestroyMenu(hMenu);
}

void CMainWindow::OnSize(UINT nType, int cx, int cy)
{
    if ((nType == SIZE_MAXIMIZED) || (nType == SIZE_RESTORED))
        OnResize(cx, cy);
}

void CMainWindow::OnSizing(UINT nSide, LPRECT lpRect)
{
    UNREFERENCED_PARAMETER(nSide);

    if (lpRect->right < (lpRect->left + MIN_MAIN_WINDOW_WIDTH))
        lpRect->right = lpRect->left + MIN_MAIN_WINDOW_WIDTH;
    if (lpRect->bottom < (lpRect->top + MIN_MAIN_WINDOW_HEIGHT))
        lpRect->bottom = lpRect->top + MIN_MAIN_WINDOW_HEIGHT;
}

void CMainWindow::OnResize(int cx, int cy)
{
    RECT rectReBar, rectStatusBar;
    if (!m_pReBar->GetClientRect(&rectReBar))
        return;
    if (!m_pStatusBar->GetClientRect(&rectStatusBar))
        return;

    HDWP hDwp = BeginDeferWindowPos(3);

    if (hDwp != NULL)
        hDwp = m_pReBar->DeferWindowPos(hDwp, NULL, 0, 0, cx, rectReBar.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
    if (hDwp != NULL)
        hDwp = m_pListView->DeferWindowPos(hDwp, NULL, 0, rectReBar.bottom, cx, cy - rectReBar.bottom - rectStatusBar.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
    if (hDwp != NULL)
        hDwp = m_pStatusBar->DeferWindowPos(hDwp, NULL, 0, cy - rectStatusBar.bottom, cx, rectStatusBar.bottom, SWP_NOACTIVATE | SWP_NOZORDER);

    if (hDwp != NULL)
    {
        EndDeferWindowPos(hDwp);
    }
    else
    {
        m_pReBar->SetWindowPos(NULL, 0, 0, cx, rectReBar.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
        m_pListView->SetWindowPos(NULL, 0, rectReBar.bottom, cx, cy - rectReBar.bottom - rectStatusBar.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
        m_pStatusBar->SetWindowPos(NULL, 0, cy - rectStatusBar.bottom, cx, rectStatusBar.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
    }

    m_pStatusBar->UpdateParts();
}

void CMainWindow::OnProgressBarShow(BOOL bVisible)
{
    UNREFERENCED_PARAMETER(bVisible);

    m_pStatusBar->UpdateParts();
}

void CMainWindow::OnSelectionChanged()
{
    UpdateToolbar();
    UpdateStatusBar();
}

void CMainWindow::OnUpdateViewToolbar()
{
    m_pReBar->GetViewToolbar()->UpdateTexts();
}

void CMainWindow::OnUpdateTitle(LPCTSTR lpszPath)
{
    LPCTSTR lpszAppTitle = g_app.GetOptions()->GetAppTitle();

    if ((lpszPath == NULL) || (*lpszPath == 0))
    {
        SetWindowText(lpszAppTitle);
    }
    else
    {
        TCHAR szTitle[MAX_TEXT];
        _stprintf_s(szTitle, MAX_TEXT, _T("%s - %s ..."), lpszAppTitle, lpszPath);
        SetWindowText(szTitle);
    }
}
