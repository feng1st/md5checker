#pragma once

class CMainToolbar
{
public:
    CMainToolbar(HWND hWndParent);
    virtual ~CMainToolbar();

public:

    virtual void EnableButton(UINT uID, BOOL bEnabled)
    {
        SendMessage(m_hWnd, TB_ENABLEBUTTON, uID, MAKELONG(bEnabled, 0));
    }

    virtual void HideButton(UINT uID, BOOL bHidden)
    {
        SendMessage(m_hWnd, TB_HIDEBUTTON, uID, MAKELONG(bHidden, 0));
    }

    virtual HWND Create();
    virtual void EnableButtons(int nSelected);

protected:
    HWND m_hWndParent;
    HWND m_hWnd;

    HIMAGELIST m_hImageList;
};
