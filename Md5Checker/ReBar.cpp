#include "stdafx.h"
#include "ReBar.h"

#include "Md5Checker.h"
#include "Resource.h"

CReBar::CReBar(HWND hWndParent)
{
    m_hWndParent = hWndParent;
    m_pMainToolbar = NULL;
    m_pViewToolbar = NULL;
}

CReBar::~CReBar()
{
    if (m_pMainToolbar != NULL)
    {
        delete m_pMainToolbar;
        m_pMainToolbar = NULL;
    }
    if (m_pViewToolbar != NULL)
    {
        delete m_pViewToolbar;
        m_pViewToolbar = NULL;
    }
}

HWND CReBar::Create()
{
    HWND hWndMainToolbar, hWndViewToolbar;

    m_pMainToolbar = new CMainToolbar(m_hWndParent);
    if ((hWndMainToolbar = m_pMainToolbar->Create()) == NULL)
        return NULL;

    m_pViewToolbar = new CViewToolbar(m_hWndParent);
    if ((hWndViewToolbar = m_pViewToolbar->Create()) == NULL)
        return NULL;

    if((m_hWnd = CreateWindow(REBARCLASSNAME, NULL, WS_CHILD | WS_CLIPCHILDREN | CCS_NODIVIDER | RBS_BANDBORDERS | RBS_VARHEIGHT, 0, 0, 0, 0, m_hWndParent, NULL, g_hInstance, NULL)) == NULL)
        return NULL;

    REBARINFO rbi;

    rbi.cbSize = sizeof(REBARINFO);
    rbi.fMask = 0;
    rbi.himl = NULL;
    if(!SendMessage(m_hWnd, RB_SETBARINFO, 0, (LPARAM)&rbi))
        return NULL;

    REBARBANDINFO rbbi;

    rbbi.cbSize = sizeof(REBARBANDINFO);
    rbbi.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;

    rbbi.fStyle = RBBS_NOGRIPPER;
    rbbi.hwndChild = hWndMainToolbar;
    rbbi.cxMinChild = 0;
    rbbi.cyMinChild = HIWORD((DWORD)SendMessage(hWndMainToolbar, TB_GETBUTTONSIZE, 0, 0));
    if (!SendMessage(m_hWnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbbi))
        return NULL;

    rbbi.fStyle = RBBS_BREAK | RBBS_NOGRIPPER;
    rbbi.hwndChild = hWndViewToolbar;
    rbbi.cxMinChild = 0;
    rbbi.cyMinChild = HIWORD((DWORD)SendMessage(hWndViewToolbar, TB_GETBUTTONSIZE, 0, 0));
    if (!SendMessage(m_hWnd, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbbi))
        return NULL;

    ShowWindow(m_hWnd, SW_SHOW);

    return m_hWnd;
}
