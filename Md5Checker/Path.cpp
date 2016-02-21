#include "stdafx.h"
#include "Path.h"

BOOL CPath::RemoveExtension()
{
    LPTSTR lpDot = _tcsrchr(m_szPath, _T('.'));
    if (lpDot == NULL)
        return FALSE;

    LPTSTR lpDelimiter;
    if ((lpDelimiter = _tcsrchr(m_szPath, _T('\\'))) == NULL)
        lpDelimiter = _tcsrchr(m_szPath, _T('/'));

    if ((lpDelimiter != NULL) && (lpDot < lpDelimiter))
        return FALSE;

    *lpDot = 0;
    return TRUE;
}
