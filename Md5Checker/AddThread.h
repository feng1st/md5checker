#pragma once

class CAddThread
{
public:
    static UINT WINAPI ThreadProc(LPVOID lpParameter);

public:
    CAddThread();
    virtual ~CAddThread();

public:

    virtual BOOL IsAdding() const
    {
        return m_bAdding;
    }

    virtual LPTSTR GetCurrentPath()
    {
        return m_szCurrentPath;
    }

public:
    virtual void Start();
    virtual void Stop();
    virtual void CheckStop();

protected:
    HANDLE m_hThread;

    BOOL m_bAdding;

    TCHAR m_szCurrentPath[MAX_PATH];
};
