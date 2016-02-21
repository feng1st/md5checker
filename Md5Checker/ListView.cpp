#include "stdafx.h"
#include "ListView.h"

#include "ClipboardHelper.h"
#include "Document.h"
#include "Md5Checker.h"
#include "CompareMD5Dialog.h"
#include "Resource.h"

#define MAX_LISTVIEW_ICONS      11

CListView::CListView(HWND hWndParent)
{
    m_hWndParent = hWndParent;
}

CListView::~CListView()
{
    m_hWnd = NULL;
}

HWND CListView::Create()
{
    if ((m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | LVS_OWNERDATA | LVS_REPORT | LVS_SHOWSELALWAYS, 0, 0, 0, 0, m_hWndParent, NULL, g_hInstance, NULL)) == NULL)
        return NULL;

    ListView_SetExtendedListViewStyle(m_hWnd, 0x00010000 /* LVS_EX_DOUBLEBUFFER */ | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

    HIMAGELIST hIconList;
    HBITMAP hBitmap;
    if ((hBitmap = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_VIEW_TOOLBAR))) == NULL)
        return NULL;
    if ((hIconList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, MAX_LISTVIEW_ICONS, 0)) == NULL)
    {
        DeleteObject(hBitmap);
        return NULL;
    }
    if (ImageList_AddMasked(hIconList, hBitmap, RGB(0, 0, 0)) == -1)
    {
        DeleteObject(hBitmap);
        return NULL;
    }
    DeleteObject(hBitmap);
    ListView_SetImageList(m_hWnd, hIconList, LVSIL_SMALL);

    TCHAR szText[MAX_CAPTION];
    LVCOLUMN lvc;

    if (LoadString(g_hInstance, IDS_NAME, szText, MAX_CAPTION) == 0)
        return NULL;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 180;
    lvc.pszText = szText;
    if (ListView_InsertColumn(m_hWnd, 0, &lvc) == -1)
        return NULL;

    if (LoadString(g_hInstance, IDS_IN_FOLDER, szText, MAX_CAPTION) == 0)
        return NULL;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 180;
    lvc.pszText = szText;
    if (ListView_InsertColumn(m_hWnd, 1, &lvc) == -1)
        return NULL;

    if (LoadString(g_hInstance, IDS_CURRENT_MD5, szText, MAX_CAPTION) == 0)
        return NULL;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 210;
    lvc.pszText = szText;
    if (ListView_InsertColumn(m_hWnd, 2, &lvc) == -1)
        return NULL;

    if (LoadString(g_hInstance, IDS_SAVED_MD5, szText, MAX_CAPTION) == 0)
        return NULL;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 210;
    lvc.pszText = szText;
    if (ListView_InsertColumn(m_hWnd, 3, &lvc) == -1)
        return NULL;

    if (LoadString(g_hInstance, IDS_SIZE, szText, MAX_CAPTION) == 0)
        return NULL;
    lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
    lvc.fmt = LVCFMT_RIGHT;
    lvc.cx = 96;
    lvc.pszText = szText;
    if (ListView_InsertColumn(m_hWnd, 4, &lvc) == -1)
        return NULL;

    if (LoadString(g_hInstance, IDS_DATE_MODIFIED, szText, MAX_CAPTION) == 0)
        return NULL;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 120;
    lvc.pszText = szText;
    if (ListView_InsertColumn(m_hWnd, 5, &lvc) == -1)
        return NULL;

    ShowWindow(m_hWnd, SW_SHOW);

    return m_hWnd;
}

void CListView::OnEditRemove()
{
    if (GetSelectedCount() == 0)
        return;

    CDocument *pDocument = g_app.GetDocument();
    CMd5ItemList *pList = pDocument->GetList();
    CMd5Item *pMd5Item;

    SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

    pDocument->AcquireWrite();

    int iCategory = pDocument->GetCategory();

    int i = pDocument->GetCount();
    CMd5ItemList::RIter iter = pList->GetLastIter();
    CMd5ItemList::RIter iterLast = iter;
    while ((pMd5Item = pList->GetPrev(iter)) != NULL)
    {
        if ((iCategory == 0) || (pMd5Item->GetCategory() == iCategory))
        {
            i--;

            if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                pDocument->DeleteMd5Item(pMd5Item);
                iter = iterLast;

                pDocument->GetCache()->Delete(i, pDocument->GetCount(), iCategory);
                DeleteItem(i);
            }
        }

        iterLast = iter;
    }

    pDocument->ReleaseWrite();

    SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);

    SetItemState(-1, 0, LVIS_SELECTED);

    g_app.SendMessage(WM_USER_MD5_ITEMS_DELETED, 0, 0);
}

void CListView::OnEditCompareMD5(BOOL bSelectionMark)
{
    CDocument *pDocument = g_app.GetDocument();

    pDocument->AcquireRead();

    int i = -1, j = -1;
    if (bSelectionMark)
    {
        if ((i = GetSelectionMark()) == -1)
        {
            pDocument->ReleaseRead();
            return;
        }
    }
    else
    {
        int nSelected = GetSelectedCount();
        if ((nSelected < 1) || (nSelected > 2))
        {
            pDocument->ReleaseRead();
            return;
        }

        if ((i = GetNextItem(-1, LVNI_SELECTED)) == -1)
        {
            pDocument->ReleaseRead();
            return;
        }

        j = GetNextItem(i, LVNI_SELECTED);
    }

    CMd5ItemCache *pCache = pDocument->GetCache();
    int iCategory = pDocument->GetCategory();
    int nCount = pDocument->GetCount();

    CCompareMD5Dialog *pCompareMD5Dialog = g_app.GetCompareMD5Dialog();
    pCompareMD5Dialog->Show();
    pCompareMD5Dialog->SendMessage(WM_USER_COMPARE_MD5,
        (WPARAM)pCache->GetMd5Item(i, nCount, iCategory),
        (LPARAM)pCache->GetMd5Item(j, nCount, iCategory)); // if i or j is equals to -1, GetMd5Item() will return NULL immediately.

    pDocument->ReleaseRead();
}

void CListView::OnEditCopy()
{
    int nSelected = GetSelectedCount();
    if (nSelected == 0)
        return;

    TCHAR szMd5Line[MAX_MD5_LINE];
    int nMd5Lines = nSelected * MAX_MD5_LINE;
    LPTSTR lpszMd5Lines = new TCHAR[nMd5Lines];
    *lpszMd5Lines = 0;

    CDocument *pDocument = g_app.GetDocument();
    CMd5ItemList *pList = pDocument->GetList();
    CMd5Item *pMd5Item;

    pDocument->AcquireRead();

    int iCategory = pDocument->GetCategory();

    int i = -1;
    CMd5ItemList::Iter iter = pList->GetFirstIter();
    while ((pMd5Item = pList->GetNext(iter)) != NULL)
    {
        if ((iCategory == 0) || (pMd5Item->GetCategory() == iCategory))
        {
            i++;

            if ((GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) && (pMd5Item->GetCopyString(szMd5Line) != NULL))
            {
                _tcsncat_s(lpszMd5Lines, nMd5Lines, szMd5Line, _TRUNCATE);
                _tcsncat_s(lpszMd5Lines, nMd5Lines, _T("\r\n"), _TRUNCATE);
            }
        }
    }

    pDocument->ReleaseRead();

    CClipboardHelper::Copy(g_app.GetMainWnd(), lpszMd5Lines);

    delete[] lpszMd5Lines;
}

void CListView::OnEditCopyMD5()
{
    int i = GetSelectionMark();
    if (i == -1)
        return;

    CDocument *pDocument = g_app.GetDocument();

    pDocument->AcquireRead();

    int iCategory = pDocument->GetCategory();

    CMd5Item *pMd5Item = pDocument->GetCache()->GetMd5Item(i, pDocument->GetCount(), iCategory);
    if (pMd5Item->HasNew())
        CClipboardHelper::Copy(g_app.GetMainWnd(), pMd5Item->GetCurrentMD5());

    pDocument->ReleaseRead();
}

void CListView::OnEditCopyPaths()
{
    int nSelected = GetSelectedCount();
    if (nSelected == 0)
        return;

    int nPaths = nSelected * (MAX_PATH + SIZE_NEWLINE);
    LPTSTR lpszPaths = new TCHAR[nPaths];
    *lpszPaths = 0;

    CDocument *pDocument = g_app.GetDocument();
    CMd5ItemList *pList = pDocument->GetList();
    CMd5Item *pMd5Item;

    pDocument->AcquireRead();

    int iCategory = pDocument->GetCategory();

    int i = -1;
    CMd5ItemList::Iter iter = pList->GetFirstIter();
    while ((pMd5Item = pList->GetNext(iter)) != NULL)
    {
        if ((iCategory == 0) || (pMd5Item->GetCategory() == iCategory))
        {
            i++;

            if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                _tcsncat_s(lpszPaths, nPaths, pMd5Item->GetFullPath(), _TRUNCATE);
                _tcsncat_s(lpszPaths, nPaths, _T("\r\n"), _TRUNCATE);
            }
        }
    }

    pDocument->ReleaseRead();

    CClipboardHelper::Copy(g_app.GetMainWnd(), lpszPaths);

    delete[] lpszPaths;
}

void CListView::OnEditDiscarded()
{
    if (GetSelectedCount() == 0)
        return;

    CDocument *pDocument = g_app.GetDocument();
    CMd5ItemList *pList = pDocument->GetList();
    CMd5Item *pMd5Item;

    SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);

    pDocument->AcquireWrite();

    int iCategory = pDocument->GetCategory();
    pMd5Item = pDocument->GetCache()->GetMd5Item(GetSelectionMark(), pDocument->GetCount(), iCategory);
    BOOL bDiscarded = pMd5Item->IsDiscarded();

    int i = -1;
    CMd5ItemList::Iter iter = pList->GetFirstIter();
    while ((pMd5Item = pList->GetNext(iter)) != NULL)
    {
        if ((iCategory == 0) || (pMd5Item->GetCategory() == iCategory))
        {
            // No coninue; before this statement to make sure the index is correct!
            i++;

            if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                if (bDiscarded)
                {
                    if (pMd5Item->IsDiscarded())
                    {
                        pMd5Item->UnmarkAsDiscarded();
                        RedrawItems(i, i);
                    }
                }
                else
                {
                    if (!pMd5Item->IsDiscarded())
                    {
                        pMd5Item->MarkAsDiscarded();
                        RedrawItems(i, i);
                    }
                }
            }
        }
    }

    pDocument->ReleaseWrite();

    SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
}

void CListView::OnEditSelectAll()
{
    SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);
}

void CListView::OnViewLocate()
{
    int i = GetSelectionMark();
    if (i == -1)
        return;

    TCHAR szParam[MAX_TEXT];

    CDocument *pDocument = g_app.GetDocument();

    pDocument->AcquireRead();

    int iCategory = pDocument->GetCategory();

    CMd5Item *pMd5Item = pDocument->GetCache()->GetMd5Item(i, pDocument->GetCount(), iCategory);

    _stprintf_s(szParam, MAX_TEXT, _T("/select, %s"), pMd5Item->GetFullPath());

    pDocument->ReleaseRead();

    ShellExecute(g_app.GetMainWnd(), _T("open"), _T("explorer"), szParam, NULL, SW_SHOW);
}

void CListView::OnViewProperties()
{
    int i = GetSelectionMark();
    if (i == -1)
        return;

    CDocument *pDocument = g_app.GetDocument();

    pDocument->AcquireRead();

    int iCategory = pDocument->GetCategory();

    CMd5Item *pMd5Item = pDocument->GetCache()->GetMd5Item(i, pDocument->GetCount(), iCategory);

    SHELLEXECUTEINFO sei;
    SecureZeroMemory(&sei, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
    sei.lpVerb = _T("properties");
    sei.lpFile = pMd5Item->GetFullPath();
    ShellExecuteEx(&sei);

    pDocument->ReleaseRead();
}
