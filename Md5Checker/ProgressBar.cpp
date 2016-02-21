#include "stdafx.h"
#include "ProgressBar.h"

#include "Md5Checker.h"
#include "Resource.h"

CProgressBar::CProgressBar(HWND hWndParent)
{
    m_hWndParent = hWndParent;
    m_bVisible = FALSE;
}

CProgressBar::~CProgressBar()
{
}

HWND CProgressBar::Create()
{
    if ((m_hWnd = CreateWindow(PROGRESS_CLASS, NULL, WS_CHILD, 0, 0, 0, 0, m_hWndParent, NULL, g_hInstance, NULL)) == NULL)
        return NULL;

    if (m_bVisible)
        ShowWindow(m_hWnd, SW_SHOW);

    return m_hWnd;
}

void CProgressBar::Show(BOOL bVisible)
{
    if (bVisible != m_bVisible)
    {
        m_bVisible = bVisible;
        SetPostion(0);
        if (m_bVisible)
            ShowWindow(m_hWnd, SW_SHOW);
        else
            ShowWindow(m_hWnd, SW_HIDE);
        g_app.SendMessage(WM_USER_PROGRESS_BAR_SHOW, m_bVisible, 0);
    }
}
