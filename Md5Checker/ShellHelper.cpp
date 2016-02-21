#include "stdafx.h"
#include "ShellHelper.h"

#ifdef UNICODE
BOOL CShellHelper::m_bGetCPInfoEx = FALSE;
CShellHelper::GETCPINFOEX CShellHelper::m_pGetCPInfoEx = NULL;
#endif

HRESULT CShellHelper::CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink, LPCTSTR lpszDesc)
{
    HRESULT hres;
    IShellLink* psl;
    WCHAR wszPathLink[MAX_PATH];

    // Get a pointer to the IShellLink interface.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description.
        psl->SetPath(lpszPathObj);
        psl->SetDescription(lpszDesc);

        // Query IShellLink for the IPersistFile interface for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres))
        {
            // Ensure that the string is Unicode.
#ifdef UNICODE
            _tcsncpy_s(wszPathLink, MAX_PATH, lpszPathLink, _TRUNCATE);
#else
            size_t stConverted;
            mbstowcs_s(&stConverted, wszPathLink, MAX_PATH, lpszPathLink, MAX_PATH);
#endif

            // Save the link by calling IPersistFile::Save.
            hres = ppf->Save(wszPathLink, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}

#ifdef UNICODE
CShellHelper::GETCPINFOEX CShellHelper::GetCPInfoEx()
{
    if (m_bGetCPInfoEx)
        return m_pGetCPInfoEx;

    HMODULE hKernel = GetModuleHandle(_T("Kernel32.dll"));
    if (hKernel != NULL)
    {
#ifdef _UNICODE
        m_pGetCPInfoEx = reinterpret_cast<GETCPINFOEX>(GetProcAddress(hKernel, "GetCPInfoExW"));
#else
        m_pGetCPInfoEx = reinterpret_cast<GETCPINFOEX>(GetProcAddress(hKernel, "GetCPInfoExA"));
#endif
    }

    m_bGetCPInfoEx = TRUE;

    return m_pGetCPInfoEx;
}

BOOL CShellHelper::GetCodePageName(UINT iCodePage, LPTSTR lpszCodePageName)
{
    if (iCodePage == CP_ACP)
        return FALSE;

    if (!IsValidCodePage(iCodePage))
        return FALSE;

    GETCPINFOEX pGetCPInfoEx = GetCPInfoEx();
    if (pGetCPInfoEx)
    {
        CPINFOEX cpInfoEx = { 0 };
        if (!pGetCPInfoEx(iCodePage, 0, &cpInfoEx))
            return FALSE;

        _tcscpy_s(lpszCodePageName, MAX_PATH, cpInfoEx.CodePageName);
    }
    else
    {
        _stprintf_s(lpszCodePageName, MAX_PATH, _T("%d"), iCodePage);
    }

    return TRUE;
}
#endif