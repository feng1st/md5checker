#pragma once

class CPath
{
public:

    CPath()
    {
        *m_szPath = 0;
    }

    CPath(LPCTSTR lpszPath)
    {
        if (lpszPath == NULL)
            *m_szPath = 0;
        else
            _tcsncpy_s(m_szPath, MAX_PATH, lpszPath, _TRUNCATE);
    }

    CPath(const CPath &path)
    {
        _tcscpy_s(m_szPath, MAX_PATH, path.m_szPath);
    }

    virtual ~CPath() { }

public:

    virtual LPCTSTR GetPath(BOOL bVerifyExists = FALSE) const
    {
        if (bVerifyExists && !PathFileExists(m_szPath))
            return NULL;
        else
            return m_szPath;
    }

public:

    virtual BOOL GetString(LPTSTR lpszString) const
    {
        if (lpszString == NULL)
            return FALSE;

        _tcscpy_s(lpszString, MAX_PATH, m_szPath);
        return TRUE;
    }

    virtual BOOL AddExtension(LPCTSTR lpszExtension)
    {
        return _tcsncat_s(m_szPath, MAX_PATH, lpszExtension, _TRUNCATE) == 0;
    }

    virtual BOOL Append(LPCTSTR lpszMore)
    {
        return PathAppend(m_szPath, lpszMore);
    }

    virtual BOOL RemoveFileSpec()
    {
        return PathRemoveFileSpec(m_szPath);
    }

    virtual BOOL RemoveExtension();

protected:
    TCHAR m_szPath[MAX_PATH];
};
