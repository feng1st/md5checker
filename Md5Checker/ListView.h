#pragma once

class CListView
{
public:
    CListView(HWND hWndParent);
    virtual ~CListView();

public:
    virtual HWND Create();

public:

    inline BOOL IsNMSender(LPNMHDR lpnmhdr) const
    {
        return (this != NULL) && (lpnmhdr->hwndFrom == m_hWnd);
    }

    virtual HDWP DeferWindowPos(HDWP hWinPosInfo, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags)
    {
        return ::DeferWindowPos(hWinPosInfo, m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
    }

    virtual BOOL DeleteAllItems()
    {
        return ListView_DeleteAllItems(m_hWnd);
    }

    virtual BOOL DeleteItem(int iItem)
    {
        return ListView_DeleteItem(m_hWnd, iItem);
    }

    virtual BOOL DeleteItemPost(int iItem)
    {
        return (BOOL)PostMessage(m_hWnd, LVM_DELETEITEM, (WPARAM)iItem, 0);
    }

    virtual int GetCountPerPage()
    {
        return ListView_GetCountPerPage(m_hWnd);
    }

    virtual int GetItemCount()
    {
        return ListView_GetItemCount(m_hWnd);
    }

    virtual UINT GetItemState(int i, UINT mask)
    {
        return ListView_GetItemState(m_hWnd, i, mask);
    }

    virtual int GetNextItem(int iStart, UINT flags)
    {
        return ListView_GetNextItem(m_hWnd, iStart, flags);
    }

    virtual UINT GetSelectedCount()
    {
        return ListView_GetSelectedCount(m_hWnd);
    }

    virtual int GetSelectionMark()
    {
        return ListView_GetSelectionMark(m_hWnd);
    }

    virtual int GetTopIndex()
    {
        return ListView_GetTopIndex(m_hWnd);
    }

    virtual int InsertItem(const LPLVITEM pitem)
    {
        return ListView_InsertItem(m_hWnd, pitem);
    }

    virtual int InsertItemPost(const LPLVITEM pitem)
    {
        return (int)PostMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)pitem);
    }

    virtual BOOL RedrawItems(int iFirst, int iLast)
    {
        return ListView_RedrawItems(m_hWnd, iFirst, iLast);
    }

    virtual BOOL RedrawItemsPost(int iFirst, int iLast)
    {
        return (BOOL)PostMessage(m_hWnd, LVM_REDRAWITEMS, (WPARAM)iFirst, (LPARAM)iLast);
    }

    virtual void SetItemCountEx(int cItems, DWORD dwFlags)
    {
        ListView_SetItemCountEx(m_hWnd, cItems, dwFlags);
    }

    virtual void SetItemState(int i, UINT state, UINT mask)
    {
        ListView_SetItemState(m_hWnd, i, state, mask);
    }

    virtual void SetRedraw(BOOL bRedraw)
    {
        SendMessage(m_hWnd, WM_SETREDRAW, bRedraw, 0);
    }

    virtual BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
    {
        return ::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }

public:
    virtual void OnEditRemove();
    virtual void OnEditCompareMD5(BOOL bSelectionMark = FALSE);
    virtual void OnEditCopy();
    virtual void OnEditCopyMD5();
    virtual void OnEditCopyPaths();
    virtual void OnEditDiscarded();
    virtual void OnEditSelectAll();
    virtual void OnViewLocate();
    virtual void OnViewProperties();

protected:
    HWND m_hWndParent;
    HWND m_hWnd;
};
