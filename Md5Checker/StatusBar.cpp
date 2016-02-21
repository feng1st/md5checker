#include "stdafx.h"
#include "StatusBar.h"

#include "Md5Checker.h"
#include "Resource.h"

CStatusBar::CStatusBar(HWND hWndParent)
{
    m_hWndParent = hWndParent;
    m_pProgressBar = NULL;
}

CStatusBar::~CStatusBar()
{
    if (m_pProgressBar != NULL)
    {
        delete m_pProgressBar;
        m_pProgressBar = NULL;
    }
}

HWND CStatusBar::Create()
{
    if (LoadString(g_hInstance, IDS_STATUS_BAR_UNSELECTED, m_szUnselected, MAX_TEXT) == NULL)
        return NULL;
    if (LoadString(g_hInstance, IDS_STATUS_BAR_SELECTED, m_szSelected, MAX_TEXT) == NULL)
        return NULL;
    if (LoadString(g_hInstance, IDS_STATUS_BAR_ADDING, m_szAdding, MAX_TEXT) == NULL)
        return NULL;
    if (LoadString(g_hInstance, IDS_STATUS_BAR_CHECKING, m_szChecking, MAX_TEXT) == NULL)
        return NULL;

    m_pProgressBar = new CProgressBar(m_hWndParent);
    if (m_pProgressBar->Create() == NULL)
        return NULL;

    if ((m_hWnd = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, m_hWndParent, NULL, g_hInstance, NULL)) == NULL)
        return NULL;

    int nWidths[] = {0, -1};
    if (!SendMessage(m_hWnd, SB_SETPARTS, 2, (LPARAM)nWidths))
        return NULL;

    if (!SendMessage(m_hWnd, SB_SETTEXT, 1 | SBT_NOBORDERS, NULL))
        return NULL;

    ShowWindow(m_hWnd, SW_SHOW);

    return m_hWnd;
}

void CStatusBar::UpdateParts()
{
    RECT rectProgressBar;
    if (!SendMessage(m_hWnd, SB_GETRECT, 1, (LPARAM)&rectProgressBar))
        return;
    int nWidths[] = {rectProgressBar.right, -1};
    if (m_pProgressBar->IsVisible())
    {
        if (nWidths[0] > MAX_PROGRESS_BAR_WIDTH)
            nWidths[0] -= MAX_PROGRESS_BAR_WIDTH;
        else
            nWidths[0] = 0;
        if (!SendMessage(m_hWnd, SB_SETPARTS, 2, (LPARAM)nWidths))
            return;
        if (!SendMessage(m_hWnd, SB_GETRECT, 1, (LPARAM)&rectProgressBar))
            return;

        RECT rectMainWindow;
        if (!::GetClientRect(m_hWndParent, &rectMainWindow))
            return;
        m_pProgressBar->SetWindowPos(NULL, rectProgressBar.left, rectMainWindow.bottom - rectProgressBar.bottom + rectProgressBar.top, rectProgressBar.right - rectProgressBar.left, rectProgressBar.bottom - rectProgressBar.top, SWP_NOACTIVATE | SWP_NOZORDER);
    }
    else
    {
        SendMessage(m_hWnd, SB_SETPARTS, 2, (LPARAM)nWidths);
    }
}

void CStatusBar::SetTextUnselected(int nTotalListed)
{
    TCHAR szText[MAX_TEXT];
    _stprintf_s(szText, MAX_TEXT, m_szUnselected, nTotalListed);
    SetText(szText);
}

void CStatusBar::SetTextSelected(int nSelected)
{
    TCHAR szText[MAX_TEXT];
    _stprintf_s(szText, MAX_TEXT, m_szSelected, nSelected);
    SetText(szText);
}

void CStatusBar::SetTextAdding(LPTSTR lpszPath)
{
    TCHAR szText[MAX_TEXT];

    if ((lpszPath != NULL) && (*lpszPath != 0))
    {
        _stprintf_s(szText, MAX_TEXT, m_szAdding, lpszPath);
        SetText(szText);

        *lpszPath = 0;
    }
}

void CStatusBar::SetTextChecking(int nChecked, int nTotal, LPCTSTR lpszFilePath)
{
    TCHAR szText[MAX_TEXT];
    _stprintf_s(szText, MAX_TEXT, m_szChecking, nChecked, nTotal, lpszFilePath);
    SetText(szText);
}
