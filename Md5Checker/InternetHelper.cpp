#include "stdafx.h"
#include "InternetHelper.h"

#include "WaitCursor.h"
#include "Wininet.h"

BOOL CInternetHelper::ReadA(LPCTSTR lpszURL, LPCTSTR lpszAgent, LPSTR lpszString, UINT nString)
{
    CWaitCursor waitCursor;

    HINTERNET hInternet = InternetOpen(lpszAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet == NULL)
        return FALSE;

    HINTERNET hURL = InternetOpenUrl(hInternet, lpszURL, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, NULL);
	if (hURL == NULL)
    {
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    DWORD dwRead;
    if (!InternetReadFile(hURL, lpszString, nString - 1, &dwRead) || (dwRead == 0))
    {
        InternetCloseHandle(hURL);
	    InternetCloseHandle(hInternet);
        return FALSE;
	}

    lpszString[dwRead] = 0;

    InternetCloseHandle(hURL);
    InternetCloseHandle(hInternet);
    return TRUE;
}
