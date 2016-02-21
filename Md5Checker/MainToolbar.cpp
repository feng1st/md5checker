#include "stdafx.h"
#include "MainToolbar.h"

#include "CommonDefine.h"
#include "Md5Checker.h"
#include "Resource.h"

#define MAX_MAIN_TOOLBAR_BUTTONS        17
#define MAX_MAIN_TOOLBAR_ICONS          12

#define INDEX_START                     8
#define INDEX_STOP                      9

static UINT uCommands[MAX_MAIN_TOOLBAR_BUTTONS] =
{
    ID_FILE_SAVE,
    ID_FILE_SAVE_EACH,
    ID_FILE_SAVE_TO,
    0,
    ID_EDIT_ADD,
    ID_EDIT_REMOVE,
    ID_EDIT_CLEAR,
    0,
    ID_EDIT_START, // INDEX_START
    ID_EDIT_STOP, // INDEX_STOP
    0,
    ID_EDIT_COPY,
    ID_EDIT_COPY_MD5,
    0,
    ID_VIEW_LOCATE,
    0,
    ID_TOOLS_OPTIONS
};

CMainToolbar::CMainToolbar(HWND hWndParent)
{
    m_hWndParent = hWndParent;
    m_hImageList = NULL;
}

CMainToolbar::~CMainToolbar()
{
    if (m_hImageList != NULL)
        ImageList_Destroy(m_hImageList);
}

HWND CMainToolbar::Create()
{
    if ((m_hWnd = CreateWindow(TOOLBARCLASSNAME, NULL, WS_CHILD | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN | TBSTYLE_FLAT, 0, 0, 0, 0, m_hWndParent, NULL, g_hInstance, NULL)) == NULL)
        return NULL;

    HBITMAP hBitmap;
    if ((hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_MAIN_TOOLBAR))) == NULL)
        return NULL;
    if ((m_hImageList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, MAX_MAIN_TOOLBAR_ICONS, 0)) == NULL)
    {
        DeleteObject(hBitmap);
        return NULL;
    }
    if (ImageList_AddMasked(m_hImageList, hBitmap, RGB(0, 0, 0)) == -1)
    {
        DeleteObject(hBitmap);
        return NULL;
    }
    DeleteObject(hBitmap);
    SendMessage(m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)m_hImageList);

    TBBUTTON tbb[MAX_MAIN_TOOLBAR_BUTTONS];
    TCHAR szText[MAX_CAPTION];
    for (int i = 0, iImage = 0; i < MAX_MAIN_TOOLBAR_BUTTONS; i++)
    {
        if ((tbb[i].idCommand = uCommands[i]) != 0)
        {
            tbb[i].iBitmap = iImage++;
            tbb[i].fsState = TBSTATE_ENABLED;
            tbb[i].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;

            if (LoadString(g_hInstance, tbb[i].idCommand, szText, MAX_CAPTION) != 0)
                tbb[i].iString = SendMessage(m_hWnd, TB_ADDSTRING, 0, (LPARAM)szText);
            else
                tbb[i].iString = -1;
        }
        else
        {
            tbb[i].iBitmap = 6;
            tbb[i].fsState = 0;
            tbb[i].fsStyle = TBSTYLE_SEP;
            tbb[i].iString = -1;
        }
        tbb[i].dwData = 0;
    }
    tbb[INDEX_STOP].fsState = TBSTATE_ENABLED | TBSTATE_HIDDEN;
    tbb[INDEX_START].fsStyle = TBSTYLE_BUTTON;
    tbb[INDEX_STOP].fsStyle = TBSTYLE_BUTTON;
    if (!SendMessage(m_hWnd, TB_ADDBUTTONS, MAX_MAIN_TOOLBAR_BUTTONS, (LPARAM)tbb))
        return NULL;

    /*
    // make Start and Stop the same size
    RECT rcStart, rcStop;
    SendMessage(m_hWnd, TB_GETITEMRECT, INDEX_START, (LPARAM)&rcStart);
    SendMessage(m_hWnd, TB_GETITEMRECT, INDEX_STOP, (LPARAM)&rcStop);
    TBBUTTONINFO tbbi;
    tbbi.cbSize = sizeof(tbbi);
    tbbi.dwMask = TBIF_SIZE;
    tbbi.cx = (WORD)max(rcStart.right - rcStart.left, rcStop.right - rcStop.left);
    SendMessage(m_hWnd, TB_SETBUTTONINFO, ID_EDIT_START, (LPARAM)&tbbi);
    SendMessage(m_hWnd, TB_SETBUTTONINFO, ID_EDIT_STOP, (LPARAM)&tbbi);

    HideButton(ID_EDIT_STOP, TRUE);
    */

    return m_hWnd;
}

void CMainToolbar::EnableButtons(int nSelected)
{
    EnableButton(ID_EDIT_REMOVE, nSelected > 0);
    EnableButton(ID_EDIT_COMPARE_MD5, (nSelected == 1) || (nSelected == 2));
    EnableButton(ID_EDIT_COPY, nSelected > 0);
    EnableButton(ID_EDIT_COPY_MD5, nSelected == 1);
    EnableButton(ID_VIEW_LOCATE, nSelected == 1);
    EnableButton(ID_VIEW_PROPERTIES, nSelected == 1);
}
