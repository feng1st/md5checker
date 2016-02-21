#pragma once

class CWildcardHelper
{
private:
    CWildcardHelper() { }

public:
    static BOOL Match(LPCTSTR lpszFileName, LPCTSTR lpszWildcards, BOOL bEmptyMatchAll);

protected:
    static BOOL DoMatch(LPCTSTR lpszFileName, LPCTSTR lpszWildcard, int nWordLength = 0);
};
