#include "stdafx.h"
#include "CheckThread.h"

#include <process.h>

#include "CommonDefine.h"
#include "Document.h"
#include "MD5Calculator.h"
#include "Md5Checker.h"
#include "Options.h"

#define TIMER_CHECKING_ELAPSE       100

#define MAX_FILE_BUFFER             4096

UINT WINAPI CCheckThread::ThreadProc(LPVOID lpParameter)
{
    CCheckThread *pThread = (CCheckThread*)lpParameter;
    CMD5Calculator md5Calculator;
    HANDLE hFile;
    BOOL bRead;
    BYTE byBuffer[MAX_FILE_BUFFER];
    DWORD dwBytesRead;
    BYTE byMD5[BYTES_MD5];
    WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    CMd5Item *pMd5Item;

    pThread->m_bSkip = FALSE;
    pThread->m_nRead.QuadPart = 0;
    while (pThread->m_bChecking)
    {
        md5Calculator.Init();

        hFile = CreateFile(pThread->m_szCurrentFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if ((bRead = (hFile != INVALID_HANDLE_VALUE)) == TRUE)
        {
            GetFileAttributesEx(pThread->m_szCurrentFilePath, GetFileExInfoStandard, &fileAttr);
            pThread->m_nFileSize.HighPart = fileAttr.nFileSizeHigh;
            pThread->m_nFileSize.LowPart = fileAttr.nFileSizeLow;
            while (pThread->m_bChecking && !pThread->m_bSkip && ((bRead = ReadFile(hFile, byBuffer, MAX_FILE_BUFFER, &dwBytesRead, NULL)) == TRUE) && (dwBytesRead > 0))
            {
                pThread->m_nRead.QuadPart += dwBytesRead;
                md5Calculator.Update(byBuffer, dwBytesRead);
            }

            CloseHandle(hFile);
        }

        if (pThread->m_bChecking && !pThread->m_bSkip)
        {
            if (bRead)
            {
                g_app.SendMessage(WM_USER_SET_CURRENT_MD5, (WPARAM)pThread->m_szCurrentFilePath, (LPARAM)md5Calculator.Finalize(byMD5));
            }
            else
            {
                if ((pMd5Item = (CMd5Item *)g_app.SendMessage(WM_USER_MARK_AS_NA, (WPARAM)pThread->m_szCurrentFilePath, 0)) != NULL)
                    pThread->m_setNAMd5Item.insert(pMd5Item);
            }
        }

        if (!pThread->GetNextFilePath())
            break;

        pThread->m_bSkip = FALSE;
        pThread->m_nRead.QuadPart = 0;
    }

    pThread->m_setNAMd5Item.clear();

    _endthreadex(0);
    return 0;
}

CCheckThread::CCheckThread()
{
    m_hThread = NULL;
    m_bChecking = FALSE;
}

CCheckThread::~CCheckThread()
{
    Stop();
}

void CCheckThread::Start()
{
    CDocument *pDocument = g_app.GetDocument();

    pDocument->AcquireRead();
    if (pDocument->GetList()->GetSize() == 0)
    {
        pDocument->ReleaseRead();
        g_app.SendMessage(WM_USER_CHECK_CANCELLED, 0, 0);
        return;
    }
    pDocument->ReleaseRead();

    CheckStop();

    if (m_hThread == NULL)
    {
        *m_szCurrentFilePath = 0;
        if (!GetNextFilePath())
        {
            g_app.SendMessage(WM_USER_CHECK_CANCELLED, 0, 0);
            return;
        }

        m_bChecking = TRUE;

        m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, NULL);
        if (g_app.GetOptions()->GetLowPriority())
            SetThreadPriority(m_hThread, THREAD_PRIORITY_LOWEST);
        else
            SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);
        ResumeThread(m_hThread);

        g_app.SendMessage(WM_USER_CHECK_STARTED, 0, 0);

        SetTimer(g_app.GetMainWnd(), TIMER_CHECKING_ID, TIMER_CHECKING_ELAPSE, NULL);

        CheckStop();
    }
}

void CCheckThread::Stop()
{
    if (m_hThread != NULL)
    {
        KillTimer(g_app.GetMainWnd(), TIMER_CHECKING_ID);

        m_bChecking = FALSE;
        WaitForSingleObject(m_hThread, INFINITE);

        g_app.SendMessage(WM_USER_CHECK_STOPPED, 0, 0);

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

void CCheckThread::CheckStop()
{
    if ((m_hThread != NULL) && (WaitForSingleObject(m_hThread, 0) == WAIT_OBJECT_0))
        Stop();
}

void CCheckThread::CheckDeleted()
{
    if ((m_hThread != NULL) && (m_bChecking) && (*m_szCurrentFilePath != 0))
    {
        CDocument *pDocument = g_app.GetDocument();
        pDocument->AcquireRead();
        if (pDocument->GetList()->GetMd5Item(m_szCurrentFilePath) == NULL)
            m_bSkip = TRUE;
        pDocument->ReleaseRead();
    }
}

BOOL CCheckThread::GetNextFilePath()
{
    CDocument *pDocument = g_app.GetDocument();
    CMd5ItemList *pList = pDocument->GetList();
    CMd5ItemList::Iter iter;
    CMd5ItemList::RIter rIter;
    CMd5Item *pMd5Item;
    BOOL bNewOnly = g_app.GetOptions()->GetCheckNewOnly();

    pDocument->AcquireRead();
    if (pList->GetSize() == 0)
    {
        pDocument->ReleaseRead();
        return FALSE;
    }
    pDocument->ReleaseRead();

    pDocument->AcquireRead();
    pMd5Item = pList->GetFirstMd5Item();
    if (IsMd5ItemUnknown(pMd5Item, bNewOnly))
    {
        _tcsncpy_s(m_szCurrentFilePath, MAX_PATH, pMd5Item->GetFullPath(), _TRUNCATE);
        pDocument->ReleaseRead();
        return TRUE;
    }
    pDocument->ReleaseRead();

    if (*m_szCurrentFilePath != 0)
    {
        pDocument->AcquireRead();
        rIter = pList->GetRIter(m_szCurrentFilePath);
        pList->GetPrev(rIter);
        pMd5Item = pList->GetPrev(rIter);
        if ((pMd5Item != NULL) && IsMd5ItemUnknown(pMd5Item, bNewOnly))
        {
            _tcsncpy_s(m_szCurrentFilePath, MAX_PATH, pMd5Item->GetFullPath(), _TRUNCATE);
            pDocument->ReleaseRead();
            return TRUE;
        }
        pDocument->ReleaseRead();

        pDocument->AcquireRead();
        iter = pList->GetIter(m_szCurrentFilePath);
        pList->GetNext(iter);
        pMd5Item = pList->GetNext(iter);
        if ((pMd5Item != NULL) && IsMd5ItemUnknown(pMd5Item, bNewOnly))
        {
            _tcsncpy_s(m_szCurrentFilePath, MAX_PATH, pMd5Item->GetFullPath(), _TRUNCATE);
            pDocument->ReleaseRead();
            return TRUE;
        }
        pDocument->ReleaseRead();
    }

    pDocument->AcquireRead();
    iter = pList->GetFirstIter();
    while ((pMd5Item = pList->GetNext(iter)) != NULL)
    {
        if (IsMd5ItemUnknown(pMd5Item, bNewOnly))
        {
            _tcsncpy_s(m_szCurrentFilePath, MAX_PATH, pMd5Item->GetFullPath(), _TRUNCATE);
            pDocument->ReleaseRead();
            return TRUE;
        }
    }
    pDocument->ReleaseRead();

    return FALSE;
}
