#pragma once

class CShellHelper
{
private:
    CShellHelper() { }

#ifdef UNICODE
protected:
    typedef BOOL (WINAPI *GETCPINFOEX)(UINT, DWORD, LPCPINFOEX);
#endif

public:
    // CreateLink - uses the Shell's IShellLink and IPersistFile interfaces
    //              to create and store a shortcut to the specified object.
    //
    // Returns the result of calling the member functions of the interfaces.
    //
    // Parameters:
    // lpszPathObj  - address of a buffer containing the path of the object.
    // lpszPathLink - address of a buffer containing the path where the
    //                Shell link is to be stored.
    // lpszDesc     - address of a buffer containing the description of the
    //                Shell link.
    static HRESULT CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink, LPCTSTR lpszDesc);
#ifdef UNICODE
    static BOOL GetCodePageName(UINT iCodePage, LPTSTR lpszCodePageName);

protected:
    static GETCPINFOEX GetCPInfoEx();

protected:
    static BOOL m_bGetCPInfoEx;
    static GETCPINFOEX m_pGetCPInfoEx;
#endif
};
