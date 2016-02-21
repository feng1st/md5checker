#pragma once

class CPathHelper
{
private:
    CPathHelper() { }

public:

    // Removes illegal characters from a file or directory path.
    // lpszClean and lpszPath can be the same buffer.
    static LPTSTR Cleanup(LPCTSTR lpszPath, LPTSTR lpszClean, LPCTSTR lpszInvalid = _T("\\/:*?\"<>|\r\n"));

    // Converts a relative path name to an absolute path name.
    // lpszAbsPath and lpszPath can be the same buffer.
    static LPTSTR Resolve(LPCTSTR lpszPath, LPTSTR lpszAbsPath, LPCTSTR lpszFolderFrom);

    // Creates a relative path from one file or folder to another.
    // lpszRelPath and lpszPath can be the same buffer.
    static LPTSTR RelatePathTo(LPCTSTR lpszPath, LPTSTR lpszRelPath, LPCTSTR lpszFolderFrom);

    // Retrieves the full path and file name of a specified file.
    // lpszFullPath and lpszPath can be the same buffer.
    static LPTSTR GetFullPath(LPCTSTR lpszPath, LPTSTR lpszFullPath, LPTSTR *lpszFilePart = NULL);

    // lpszSlashDelimited and lpszPath can be the same buffer.
    static LPTSTR GetSlashDelimitedPath(LPCTSTR lpszPath, LPTSTR lpszSlashDelimited);
};
