#pragma once

#include <queue>
#include <set>

#include "Md5File.h"
#include "Md5ItemCache.h"

using namespace std;

#define MAX_CATEGORIES      7

typedef queue<LPTSTR> STRQUEUE;
typedef map<LPTSTR, CMd5File *, _tcsiless<LPTSTR>> STRMD5FILEMAP;
typedef STRMD5FILEMAP::iterator STRMD5FILEMAPI;
typedef STRMD5FILEMAP::value_type STRMD5FILEMAPVT;
typedef list<LPTSTR> STRLIST;
typedef STRLIST::iterator STRLISTI;
typedef set<LPTSTR, _tcsiless<LPTSTR>> STRSET;
typedef STRSET::iterator STRSETI;
typedef list<CMd5Item *> MD5ITEMLIST;
typedef MD5ITEMLIST::iterator MD5ITEMLISTI;

class CDocument
{
public:
    CDocument();
    virtual ~CDocument();

public:

    virtual BOOL HasPathQueued() const
    {
        return !m_pPathQueue->empty();
    }

    virtual LPCTSTR GetQueuedPath() const
    {
        if (HasPathQueued())
            return m_pPathQueue->front();
        else
            return NULL;
    }

    virtual int GetCategory() const
    {
        return m_iCategory;
    }

    virtual int GetCount() const
    {
        return m_nCounts[m_iCategory];
    }

    virtual int GetCount(int iCategory) const
    {
        return m_nCounts[iCategory];
    }

    virtual CMd5ItemList *GetList() const
    {
        return m_pMd5ItemList;
    }

    virtual CMd5ItemCache *GetCache() const
    {
        return m_pMd5ItemCache;
    }

    virtual int SetCategory(int iCategory)
    {
        if (iCategory == m_iCategory)
            return -1;
        m_iCategory = iCategory;
        m_pMd5ItemCache->Clear();
        return m_nCounts[m_iCategory];
    }

public:

    virtual void AcquireReadPathQueue()
    {
        if (m_nPathQueueReaderCount == 0)
            EnterCriticalSection(&m_csPathQueue);
        InterlockedIncrementRelease(&m_nPathQueueReaderCount);
    }

    virtual void ReleaseReadPathQueue()
    {
        InterlockedDecrementAcquire(&m_nPathQueueReaderCount);
        if (m_nPathQueueReaderCount == 0)
            LeaveCriticalSection(&m_csPathQueue);
    }

    virtual void AcquireWritePathQueue()
    {
        EnterCriticalSection(&m_csPathQueue);
    }

    virtual void ReleaseWritePathQueue()
    {
        LeaveCriticalSection(&m_csPathQueue);
    }

    virtual void AcquireRead()
    {
        if (m_nDataReaderCount == 0)
            EnterCriticalSection(&m_csData);
        InterlockedIncrementRelease(&m_nDataReaderCount);
    }

    virtual void ReleaseRead()
    {
        InterlockedDecrementAcquire(&m_nDataReaderCount);
        if (m_nDataReaderCount == 0)
            LeaveCriticalSection(&m_csData);
    }

    virtual void AcquireWrite()
    {
        EnterCriticalSection(&m_csData);
    }

    virtual void ReleaseWrite()
    {
        LeaveCriticalSection(&m_csData);
    }

    virtual BOOL QueuePath(LPCTSTR lpszPath)
    {
        LPTSTR lpszPathCopy;
        if ((lpszPathCopy = new TCHAR[MAX_PATH]) == NULL)
            return FALSE;

        _tcsncpy_s(lpszPathCopy, MAX_PATH, lpszPath, _TRUNCATE);
        m_pPathQueue->push(lpszPathCopy);
        return TRUE;
    }

    virtual void DequeuePath()
    {
        delete[] m_pPathQueue->front();
        m_pPathQueue->pop();
    }

    virtual void ClearPathQueue();
    virtual void ParsePaths(LPTSTR lpszPaths, BOOL bShareDirectory);

    virtual void AddPaths(const BOOL *pcbAdding, LPTSTR lpszCurrentPath);
    virtual BOOL LoadAddFile(LPCTSTR lpszFolderPath, LPCTSTR lpszFullPath);

    virtual void Clear();
    virtual void ClearDiscarded();
    virtual void Save();
    virtual void Save(LPCTSTR lpszFilePath);
    virtual void SaveEach();
    virtual void SetMd5ItemCurrentMD5(LPCTSTR lpszFilePath, BYTE byMD5[]);
    virtual CMd5Item *MarkMd5ItemAsNA(LPCTSTR lpszFilePath);
    virtual void InsertMd5Item(CMd5Item *pMd5Item, BOOL bCheckExist);
    virtual void DeleteMd5Item(CMd5Item *pMd5Item);
    virtual void DeleteFromView(CMd5Item *pMd5Item);
    virtual void InsertToView(CMd5Item *pMd5Item);
    virtual void UpdateInViewAll(CMd5Item *pMd5Item);
    virtual void Sort(int iSort, BOOL bAsc);

protected:

    virtual BOOL AddNormalFile(LPCTSTR lpszFilePath)
    {
        if ((lpszFilePath == NULL) || (*lpszFilePath == 0))
            return FALSE;

        CMd5Item *pMd5Item = new CMd5Item(lpszFilePath);
        AddMd5Item(pMd5Item);

        return TRUE;
    }

    virtual void AddPath(LPCTSTR lpszPath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath);
    virtual void AddFolder(LPCTSTR lpszFolderPath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath);
    virtual void AddNormalFile(LPCTSTR lpszFilePath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath);
    virtual void AddMd5File(LPCTSTR lpszMd5FilePath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath);
    virtual void AddMd5Item(CMd5Item *pMd5Item);
    virtual BOOL LoadFromMd5File(LPCTSTR lpszFolderPath, LPCTSTR lpszMd5FileName, LPCTSTR lpszFullPath);

    virtual BOOL GetFolderPath(LPCTSTR lpszFilePath, LPTSTR lpszFolderPath);
    virtual BOOL GetMd5FilePath(LPCTSTR lpszFolderPath, LPTSTR lpszMd5FilePath);
    virtual void CleanUpMd5File(LPCTSTR lpszMd5FilePath);

public:
    virtual void OnGetDispInfo(LPNMLVDISPINFO lpmn);
    virtual void OnODCacheHint(LPNMLVCACHEHINT lpnm);

protected:
    LONG m_nPathQueueReaderCount;
    LONG m_nDataReaderCount;
    CRITICAL_SECTION m_csPathQueue;
    CRITICAL_SECTION m_csData;

    STRQUEUE *m_pPathQueue;

    int m_iCategory;
    int m_nCounts[MAX_CATEGORIES];
    CMd5ItemList *m_pMd5ItemList;
    CMd5ItemCache *m_pMd5ItemCache;
};
