#include "stdafx.h"
#include "AddThread.h"

#include <process.h>

#include "CommonDefine.h"
#include "Document.h"
#include "Md5Checker.h"

#define TIMER_ADDING_ELAPSE     100

UINT WINAPI CAddThread::ThreadProc(LPVOID lpParameter)
{
    CAddThread *pThread = (CAddThread *)lpParameter;

    *pThread->m_szCurrentPath = 0;

    g_app.GetDocument()->AddPaths(&pThread->m_bAdding, pThread->m_szCurrentPath);

    pThread->m_bAdding = FALSE;

    _endthreadex(0);
    return 0;
}

CAddThread::CAddThread()
{
    m_hThread = NULL;
    m_bAdding = FALSE;
}

CAddThread::~CAddThread()
{
    Stop();
}

void CAddThread::Start()
{
    if (IsAdding())
        return;

    CDocument *pDocument;
    pDocument = g_app.GetDocument();

    pDocument->AcquireReadPathQueue();
    if (!pDocument->HasPathQueued())
    {
        pDocument->ReleaseReadPathQueue();
        g_app.SendMessage(WM_USER_ADD_CANCELLED, 0, 0);
        return;
    }
    pDocument->ReleaseReadPathQueue();

    CheckStop();

    if (m_hThread == NULL)
    {
        m_bAdding = TRUE;

        m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, NULL);
        if (g_app.GetOptions()->GetLowPriority())
            SetThreadPriority(m_hThread, THREAD_PRIORITY_LOWEST);
        else
            SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);
        ResumeThread(m_hThread);

        g_app.SendMessage(WM_USER_ADD_STARTED, 0, 0);

        SetTimer(g_app.GetMainWnd(), TIMER_ADDING_ID, TIMER_ADDING_ELAPSE, NULL);

        CheckStop();
    }
}

void CAddThread::Stop()
{
    if (m_hThread != NULL)
    {
        KillTimer(g_app.GetMainWnd(), TIMER_ADDING_ID);

        m_bAdding = FALSE;
        WaitForSingleObject(m_hThread, INFINITE);

        g_app.SendMessage(WM_USER_ADD_STOPPED, 0, 0);

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

void CAddThread::CheckStop()
{
    if ((m_hThread != NULL) && (WaitForSingleObject(m_hThread, 0) == WAIT_OBJECT_0))
        Stop();
}
