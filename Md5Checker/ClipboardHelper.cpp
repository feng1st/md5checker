#include "stdafx.h"
#include "ClipboardHelper.h"

BOOL CClipboardHelper::Copy(HWND hWnd, LPCTSTR lpszText)
{
    if ((hWnd == NULL) || (lpszText == NULL))
        return FALSE;

    if (!OpenClipboard(hWnd))
        return FALSE;
    EmptyClipboard();

    int cch = (int)_tcslen(lpszText);

    HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR));
    if (hglbCopy == NULL)
    {
        CloseClipboard();
        return FALSE;
    }

    LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);
    memcpy(lptstrCopy, lpszText, cch * sizeof(TCHAR));
    lptstrCopy[cch] = (TCHAR)0;
    GlobalUnlock(hglbCopy);

#ifdef UNICODE
    SetClipboardData(CF_UNICODETEXT, hglbCopy);
#else
    SetClipboardData(CF_TEXT, hglbCopy);

    hglbCopy = GlobalAlloc(GMEM_MOVEABLE, sizeof(LCID));
    if (hglbCopy == NULL)
    {
        CloseClipboard();
        return FALSE;
    }

    *((LCID*)GlobalLock(hglbCopy)) = LOCALE_USE_CP_ACP;
    GlobalUnlock(hglbCopy);

    SetClipboardData(CF_LOCALE, hglbCopy);
#endif

    CloseClipboard();

    return TRUE;
}

BOOL CClipboardHelper::Paste(HWND hWnd, LPTSTR lpszText, int nText)
{
#ifdef UNICODE
    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
#else
    if (!IsClipboardFormatAvailable(CF_TEXT))
#endif
        return FALSE;

    if (!OpenClipboard(hWnd))
        return FALSE;

#ifdef UNICODE
    HGLOBAL hglbPaste = GetClipboardData(CF_UNICODETEXT);
#else
    HGLOBAL hglbPaste = GetClipboardData(CF_TEXT);
#endif
    if (hglbPaste == NULL)
    {
        CloseClipboard();
        return FALSE;
    }

    LPCTSTR lpszPaste = (LPCTSTR)GlobalLock(hglbPaste);
    if (lpszPaste == NULL)
    {
        CloseClipboard();
        return FALSE;
    }

    _tcsncpy_s(lpszText, nText, lpszPaste, _TRUNCATE);

    GlobalUnlock(hglbPaste);

    CloseClipboard();

    return TRUE;
}
