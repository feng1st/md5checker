#pragma once

#include "CommonDefine.h"
#include "Document.h"

class CViewToolbar
{
public:
    CViewToolbar(HWND hWndParent);
    virtual ~CViewToolbar();

public:

    virtual void CheckButton(UINT uID, BOOL bChecked = TRUE)
    {
        SendMessage(m_hWnd, TB_CHECKBUTTON, uID, MAKELONG(bChecked, 0));
    }

    virtual HWND Create();
    virtual void UpdateTexts();

protected:
    HWND m_hWndParent;
    HWND m_hWnd;

    HIMAGELIST m_hImageList;

    int m_nCounts[MAX_CATEGORIES];
    TCHAR m_szFormats[MAX_CATEGORIES][MAX_CAPTION];
};
