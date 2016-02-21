#include "stdafx.h"
#include "Document.h"

#include <io.h>

#include "Md5Checker.h"
#include "Md5ItemSortHelper.h"
#include "MessageBoxHelper.h"
#include "Options.h"
#include "Path.h"
#include "Resource.h"
#include "WildcardHelper.h"

#define MAX_OPEN_FILE       64

CDocument::CDocument()
{
    m_nPathQueueReaderCount = 0;
    m_nDataReaderCount = 0;
    InitializeCriticalSection(&m_csPathQueue);
    InitializeCriticalSection(&m_csData);

    m_pPathQueue = new STRQUEUE();

    m_iCategory = 0;
    for (int i = 0; i < MAX_CATEGORIES; i++)
        m_nCounts[i] = 0;

    m_pMd5ItemList = new CMd5ItemList();
    m_pMd5ItemCache = new CMd5ItemCache(m_pMd5ItemList);
}

CDocument::~CDocument()
{
    Clear();

    delete m_pMd5ItemCache;
    delete m_pMd5ItemList;

    delete m_pPathQueue;

    DeleteCriticalSection(&m_csData);
    DeleteCriticalSection(&m_csPathQueue);
}

void CDocument::ClearPathQueue()
{
    while (HasPathQueued())
        DequeuePath();
}

void CDocument::ParsePaths(LPTSTR lpszPaths, BOOL bShareDirectory)
{
    if ((lpszPaths == NULL) || (*lpszPaths == 0))
        return;

    AcquireWritePathQueue();

    LPTSTR lpszPath;
    if (bShareDirectory) // Paths from Open File dialog
    {
        if (PathIsDirectory(lpszPaths)) // Format: A shared folder path followed by separated file names
        {
            TCHAR szPath[MAX_PATH];
            _tcsncpy_s(szPath, MAX_PATH, lpszPaths, _TRUNCATE); // Get the shared folder path
            int n = (int)_tcslen(lpszPaths);
            lpszPath = lpszPaths + n + 1; // Get the first separated file name
            do
            {
                szPath[n] = 0;
                if (PathAppend(szPath, lpszPath) && PathFileExists(szPath)) // Make full path and verify existence
                    QueuePath(szPath);

                lpszPath += _tcslen(lpszPath) + 1; // Get the next separated file name
            } while (*lpszPath != 0);
        }
        else if (PathFileExists(lpszPaths)) // Format: Only one file path
        {
            QueuePath(lpszPaths);
        }
        else // Format: A folder path followed by '(Open current folder)'
        {
            LPCTSTR lpszOFNFolder = g_app.GetOptions()->GetOFNFolder();
            lpszPath = _tcsstr(lpszPaths, lpszOFNFolder);
            if ((lpszPath != NULL) && ((int)(_tcslen(lpszPaths) - _tcslen(lpszOFNFolder)) == (lpszPath - lpszPaths))) // Verify format
            {
                *lpszPath = 0;
                if (PathIsDirectory(lpszPaths))
                    QueuePath(lpszPaths);
            }
        }
    }
    else // Format: Individual paths
    {
        lpszPath = lpszPaths;
        do
        {
            if (PathFileExists(lpszPath))
                QueuePath(lpszPath);
            lpszPath += _tcslen(lpszPath) + 1;
        } while (*lpszPath != 0);
    }

    ReleaseWritePathQueue();
}

void CDocument::AddPaths(const BOOL *pcbAdding, LPTSTR lpszCurrentPath)
{
    LPCTSTR lpszPath;
    TCHAR szPath[MAX_PATH];

    AcquireReadPathQueue();
    while (pcbAdding && ((lpszPath = GetQueuedPath()) != NULL))
    {
        _tcsncpy_s(szPath, MAX_PATH, lpszPath, _TRUNCATE);
        ReleaseReadPathQueue();

        AddPath(szPath, pcbAdding, lpszCurrentPath);

        AcquireWritePathQueue();
        DequeuePath();
        ReleaseWritePathQueue();

        AcquireReadPathQueue();
    }
    ReleaseReadPathQueue();

    AcquireWritePathQueue();
    ClearPathQueue();
    ReleaseWritePathQueue();
}

BOOL CDocument::LoadAddFile(LPCTSTR lpszFolderPath, LPCTSTR lpszFullPath)
{
    if ((lpszFolderPath == NULL) || (*lpszFolderPath == 0) ||
        (lpszFullPath == NULL) || (*lpszFullPath == 0))
        return FALSE;

    CPath findPath(lpszFolderPath);
    HANDLE hFind;
    WIN32_FIND_DATA findData;

    if (findPath.Append(_T("*")) && findPath.AddExtension(MD5_EXTENSION))
    {
        hFind = FindFirstFile(findPath.GetPath(), &findData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (LoadFromMd5File(lpszFolderPath, findData.cFileName, lpszFullPath))
                    {
                        FindClose(hFind);
                        return TRUE;
                    }
                }
            } while (FindNextFile(hFind, &findData));

            FindClose(hFind);
        }
    }

    return AddNormalFile(lpszFullPath);
}

void CDocument::Clear()
{
    AcquireWritePathQueue();
    ClearPathQueue();
    ReleaseWritePathQueue();

    AcquireWrite();

    if (m_pMd5ItemList->GetSize() == 0)
    {
        ReleaseWrite();
        return;
    }

    m_pMd5ItemCache->Clear();

    m_pMd5ItemList->Clear();

    for (int i = 0; i < MAX_CATEGORIES; i++)
        m_nCounts[i] = 0;

    ReleaseWrite();
}

void CDocument::ClearDiscarded()
{
    CListView *pListView = g_app.GetMainWindow()->GetListView();

    pListView->SetRedraw(FALSE);

    AcquireWrite();

    CMd5Item *pMd5Item;
    BOOL bListed;
    int i = m_nCounts[m_iCategory];
    CMd5ItemList::RIter iter = m_pMd5ItemList->GetLastIter();
    CMd5ItemList::RIter iterLast = iter;
    while ((pMd5Item = m_pMd5ItemList->GetPrev(iter)) != NULL)
    {
        bListed = (m_iCategory == 0) || (pMd5Item->GetCategory() == m_iCategory);
        if (bListed)
            i--;

        if (pMd5Item->IsDiscarded())
        {
            DeleteMd5Item(pMd5Item);
            iter = iterLast;

            if (bListed)
            {
                GetCache()->Delete(i, m_nCounts[m_iCategory], m_iCategory);
                pListView->DeleteItem(i);
            }
        }

        iterLast = iter;
    }

    ReleaseWrite();

    pListView->SetRedraw(TRUE);
}

void CDocument::Save()
{
    STRSET setFolder;
    STRSETI iterFolder;
    STRMD5FILEMAP mapOpenedMd5File;
    STRMD5FILEMAPI iterOpenedMd5File;
    TCHAR szMd5FilePath[MAX_PATH];
    TCHAR szFolderPath[MAX_PATH];
    CMd5File *pMd5File;

    CMd5Item *pMd5Item;
    CMd5ItemList::RIter iter = m_pMd5ItemList->GetLastIter();
    while ((pMd5Item = m_pMd5ItemList->GetPrev(iter)) != NULL)
    {
        if (pMd5Item->HasNew() || pMd5Item->HasOld())
        {
            if (!GetFolderPath(pMd5Item->GetFullPath(), szFolderPath))  // it is by design that its MD5 will lost if its folder path is invalid
                continue;

            if (!GetMd5FilePath(szFolderPath, szMd5FilePath))
                continue;

            if ((iterFolder = setFolder.find(szFolderPath)) == setFolder.end())
            {
                if (PathFileExists(szMd5FilePath))
                    CleanUpMd5File(szMd5FilePath);

                LPTSTR lpszFolderPath = new TCHAR[MAX_PATH];
                _tcsncpy_s(lpszFolderPath, MAX_PATH, szFolderPath, _TRUNCATE);
                iterFolder = setFolder.insert(lpszFolderPath).first;
            }

            if (pMd5Item->IsDiscarded())
                continue;

            if ((iterOpenedMd5File = mapOpenedMd5File.find(szFolderPath)) == mapOpenedMd5File.end())
            {
                if (mapOpenedMd5File.size() >= MAX_OPEN_FILE)
                {
                    iterOpenedMd5File = mapOpenedMd5File.begin();
                    pMd5File = iterOpenedMd5File->second;
                    mapOpenedMd5File.erase(iterOpenedMd5File);
                    pMd5File->Close();
                    delete pMd5File;

                    iterOpenedMd5File = mapOpenedMd5File.end();
                    iterOpenedMd5File--;
                    pMd5File = iterOpenedMd5File->second;
                    mapOpenedMd5File.erase(iterOpenedMd5File);
                    pMd5File->Close();
                    delete pMd5File;
                }

                pMd5File = new CMd5File();
                if (!pMd5File->Open(szMd5FilePath, CMd5File::ModeAppend | CMd5File::WritePathFileName))
                {
                    delete pMd5File;
                    continue;
                }

                iterOpenedMd5File = mapOpenedMd5File.insert(STRMD5FILEMAPVT(*iterFolder, pMd5File)).first;
            }

            pMd5File = iterOpenedMd5File->second;
            if (pMd5File->Write(pMd5Item))
            {
                if (pMd5Item->IsNew() || pMd5Item->IsFailed())
                {
                    DeleteFromView(pMd5Item);
                    pMd5Item->UpdateMD5();
                    InsertToView(pMd5Item);
                    UpdateInViewAll(pMd5Item);
                }
            }
        }
    }

    for (iterOpenedMd5File = mapOpenedMd5File.begin(); iterOpenedMd5File != mapOpenedMd5File.end(); iterOpenedMd5File++)
    {
        pMd5File = iterOpenedMd5File->second;
        pMd5File->Close();
        delete pMd5File;
    }
    mapOpenedMd5File.clear();

    WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    int n = 0;
    for (iterFolder = setFolder.begin(); iterFolder != setFolder.end(); iterFolder++)
    {
        if (GetMd5FilePath(*iterFolder, szMd5FilePath))
        {
            if (GetFileAttributesEx(szMd5FilePath, GetFileExInfoStandard, &fileAttr) && (fileAttr.nFileSizeHigh == 0) && (fileAttr.nFileSizeLow == 0))
                DeleteFile(szMd5FilePath);
            else if (n++ == 0)
                g_app.SendMessage(WM_USER_UPDATE_TITLE, (WPARAM)szMd5FilePath, 0);
        }
        delete[] *iterFolder;
    }
    if (n == 0)
        g_app.SendMessage(WM_USER_UPDATE_TITLE, NULL, 0);
    setFolder.clear();
}

void CDocument::Save(LPCTSTR lpszFilePath)
{
    CMd5File md5File;
    BOOL bWritten = FALSE;
    CMd5Item *pMd5Item;
    CMd5ItemList::RIter iter;
    if (md5File.Open(lpszFilePath, CMd5File::ModeWrite | (g_app.GetOptions()->GetSaveToFullPath() ? CMd5File::WritePathFull : CMd5File::WritePathRelative)))
    {
        iter = m_pMd5ItemList->GetLastIter();
        while ((pMd5Item = m_pMd5ItemList->GetPrev(iter)) != NULL)
        {
            if (!pMd5Item->IsDiscarded() && (pMd5Item->HasNew() || pMd5Item->HasOld()))
            {
                if (md5File.Write(pMd5Item))
                {
                    if (pMd5Item->IsNew() || pMd5Item->IsFailed())
                    {
                        DeleteFromView(pMd5Item);
                        pMd5Item->UpdateMD5();
                        InsertToView(pMd5Item);
                        UpdateInViewAll(pMd5Item);
                    }

                    bWritten = TRUE;
                }
            }
        }
        md5File.Close();

        if (bWritten)
        {
            g_app.SendMessage(WM_USER_UPDATE_TITLE, (WPARAM)lpszFilePath, 0);
        }
        else
        {
            g_app.SendMessage(WM_USER_UPDATE_TITLE, NULL, 0);
            DeleteFile(lpszFilePath);
        }
    }
    else
    {
        CMessageBoxHelper::Show(g_hInstance, g_app.GetMainWnd(), IDS_SAVING_FAILED, IDS_APP_TITLE, MB_ICONWARNING | MB_OK, lpszFilePath);
    }
}

void CDocument::SaveEach()
{
    CMd5File md5File;
    TCHAR szMd5FilePath[MAX_PATH];
    TCHAR szFolderPath[MAX_PATH];
    CMd5Item *pMd5Item;
    CMd5ItemList::RIter iter = m_pMd5ItemList->GetLastIter();
    int n = 0;
    while ((pMd5Item = m_pMd5ItemList->GetPrev(iter)) != NULL)
    {
        if (pMd5Item->HasNew() || pMd5Item->HasOld())
        {
            if (!GetFolderPath(pMd5Item->GetFullPath(), szFolderPath))  // it is by design that its MD5 will lost if its folder path is invalid
                continue;

            if ((_tcsncpy_s(szMd5FilePath, MAX_PATH, pMd5Item->GetFullPath(), _TRUNCATE) == 0) &&
                (_tcsncat_s(szMd5FilePath, MAX_PATH, MD5_EXTENSION, _TRUNCATE) == 0))
            {
                if (!pMd5Item->IsDiscarded())
                {
                    if (md5File.Open(szMd5FilePath, CMd5File::ModeWrite | CMd5File::WritePathFileName))
                    {
                        md5File.Write(pMd5Item);
                        md5File.Close();

                        if (n++ == 0)
                            g_app.SendMessage(WM_USER_UPDATE_TITLE, (WPARAM)szMd5FilePath, 0);

                        if (pMd5Item->IsNew() || pMd5Item->IsFailed())
                        {
                            DeleteFromView(pMd5Item);
                            pMd5Item->UpdateMD5();
                            InsertToView(pMd5Item);
                            UpdateInViewAll(pMd5Item);
                        }
                    }
                }
                else
                {
                    DeleteFile(szMd5FilePath);
                }
            }
        }
    }
    if (n == 0)
        g_app.SendMessage(WM_USER_UPDATE_TITLE, NULL, 0);
}

void CDocument::SetMd5ItemCurrentMD5(LPCTSTR lpszFilePath, BYTE byMD5[])
{
    AcquireWrite();
    CMd5Item *pMd5Item = GetList()->GetMd5Item(lpszFilePath);
    if (pMd5Item != NULL)
    {
        DeleteFromView(pMd5Item);
        pMd5Item->SetCurrentMD5(byMD5);
        InsertToView(pMd5Item);
        UpdateInViewAll(pMd5Item);
    }
    ReleaseWrite();
}

CMd5Item *CDocument::MarkMd5ItemAsNA(LPCTSTR lpszFilePath)
{
    AcquireWrite();
    CMd5Item *pMd5Item = GetList()->GetMd5Item(lpszFilePath);
    if (pMd5Item != NULL)
    {
        DeleteFromView(pMd5Item);
        pMd5Item->MarkAsNA();
        InsertToView(pMd5Item);
        UpdateInViewAll(pMd5Item);
    }
    ReleaseWrite();

    return pMd5Item;
}

void CDocument::InsertMd5Item(CMd5Item *pMd5Item, BOOL bCheckExist)
{
    int iCategory = pMd5Item->GetCategory();
    if (m_pMd5ItemList->Insert(pMd5Item, bCheckExist))
    {
        m_nCounts[iCategory]++;
        m_nCounts[0]++;
    }
}

void CDocument::DeleteMd5Item(CMd5Item *pMd5Item)
{
    int iCategory = pMd5Item->GetCategory();
    if (m_pMd5ItemList->Delete(pMd5Item))
    {
        m_nCounts[iCategory]--;
        m_nCounts[0]--;
    }
}

void CDocument::DeleteFromView(CMd5Item *pMd5Item)
{
    m_nCounts[pMd5Item->GetCategory()]--;

    if (pMd5Item->GetCategory() != m_iCategory)
        return;

    int i = m_pMd5ItemCache->GetIndex(pMd5Item, m_iCategory);
    if (i != -1)
    {
        m_pMd5ItemCache->Delete(i, m_nCounts[m_iCategory], m_iCategory, FALSE);
        g_app.GetMainWindow()->GetListView()->DeleteItem(i);
    }
}

void CDocument::InsertToView(CMd5Item *pMd5Item)
{
    static LVITEM lvi;

    m_nCounts[pMd5Item->GetCategory()]++;

    if (pMd5Item->GetCategory() != m_iCategory)
        return;

    int i = m_pMd5ItemCache->GetIndex(pMd5Item, m_iCategory);
    if (i != -1)
    {
        m_pMd5ItemCache->Insert(i, m_nCounts[m_iCategory], m_iCategory, FALSE);

        lvi.mask = 0;
        lvi.iItem = i;
        lvi.iSubItem = 0;
        g_app.GetMainWindow()->GetListView()->InsertItem(&lvi);
    }
}

void CDocument::UpdateInViewAll(CMd5Item *pMd5Item)
{
    if (m_iCategory != 0)
        return;

    int i = m_pMd5ItemCache->GetIndex(pMd5Item, CACHED_ONLY_CATEGORY);
    if (i != -1)
        g_app.GetMainWindow()->GetListView()->RedrawItems(i, i);
}

void CDocument::Sort(int iSort, BOOL bAsc)
{
    switch (iSort)
    {
    case 0:
        if (bAsc)
            CMd5ItemSortHelper::Sort<_tmd5filenameless>(m_pMd5ItemList, m_iCategory);
        else
            CMd5ItemSortHelper::Sort<_tmd5filenamegreater>(m_pMd5ItemList, m_iCategory);
        break;

    case 1:
        if (bAsc)
            CMd5ItemSortHelper::Sort<_tmd5folderpathless>(m_pMd5ItemList, m_iCategory);
        else
            CMd5ItemSortHelper::Sort<_tmd5folderpathgreater>(m_pMd5ItemList, m_iCategory);
        break;

    case 2:
        if (bAsc)
            CMd5ItemSortHelper::Sort<_tmd5currentmd5less>(m_pMd5ItemList, m_iCategory);
        else
            CMd5ItemSortHelper::Sort<_tmd5currentmd5greater>(m_pMd5ItemList, m_iCategory);
        break;

    case 3:
        if (bAsc)
            CMd5ItemSortHelper::Sort<_tmd5savedmd5less>(m_pMd5ItemList, m_iCategory);
        else
            CMd5ItemSortHelper::Sort<_tmd5savedmd5greater>(m_pMd5ItemList, m_iCategory);
        break;

    case 4:
        if (bAsc)
            CMd5ItemSortHelper::Sort<_tmd5sizeless>(m_pMd5ItemList, m_iCategory);
        else
            CMd5ItemSortHelper::Sort<_tmd5sizegreater>(m_pMd5ItemList, m_iCategory);
        break;

    case 5:
        if (bAsc)
            CMd5ItemSortHelper::Sort<_tmd5modifiedless>(m_pMd5ItemList, m_iCategory);
        else
            CMd5ItemSortHelper::Sort<_tmd5modifiedgreater>(m_pMd5ItemList, m_iCategory);
        break;
    }
}

void CDocument::AddPath(LPCTSTR lpszPath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath)
{
    if (PathIsDirectory(lpszPath))
    {
        g_app.SendMessage(WM_USER_UPDATE_TITLE, (WPARAM)lpszPath, 0);
        AddFolder(lpszPath, pcbAdding, lpszCurrentPath);
    }
    else if (_tcsicmp(PathFindExtension(lpszPath), MD5_EXTENSION) == 0)
    {
        g_app.SendMessage(WM_USER_UPDATE_TITLE, (WPARAM)lpszPath, 0);
        AddMd5File(lpszPath, pcbAdding, lpszCurrentPath);
    }
    else
    {
        AddNormalFile(lpszPath, pcbAdding, lpszCurrentPath);
    }
}

void CDocument::AddFolder(LPCTSTR lpszFolderPath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath)
{
    if (!*pcbAdding)
        return;

    if (*lpszCurrentPath == 0)
        _tcsncpy_s(lpszCurrentPath, MAX_PATH, lpszFolderPath, _TRUNCATE);

    COptions *pOptions = g_app.GetOptions();
    TCHAR szFind[MAX_PATH];
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    TCHAR szPath[MAX_PATH];

    _tcsncpy_s(szFind, MAX_PATH, lpszFolderPath, _TRUNCATE);
    if (PathAppend(szFind, _T("*")))
    {
        hFind = FindFirstFile(szFind, &findData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (pOptions->GetRecurse() && (_tcsspnp(findData.cFileName, _T(".")) != NULL))
                    {
                        _tcsncpy_s(szPath, MAX_PATH, lpszFolderPath, _TRUNCATE);
                        if (PathAppend(szPath, findData.cFileName))
                            AddFolder(szPath, pcbAdding, lpszCurrentPath);
                    }
                }
                else
                {
                    if ((_tcsicmp(PathFindExtension(findData.cFileName), MD5_EXTENSION) == 0) ||
                        (CWildcardHelper::Match(findData.cFileName, pOptions->GetInclude(), TRUE) &&
                        !CWildcardHelper::Match(findData.cFileName, pOptions->GetExclude(), FALSE)))
                    {
                        _tcsncpy_s(szPath, MAX_PATH, lpszFolderPath, _TRUNCATE);
                        if (PathAppend(szPath, findData.cFileName))
                        {
                            if (_tcsicmp(PathFindExtension(szPath), MD5_EXTENSION) == 0)
                                AddMd5File(szPath, pcbAdding, lpszCurrentPath);
                            else
                                AddNormalFile(szPath, pcbAdding, lpszCurrentPath);
                        }
                    }
                }
            } while (*pcbAdding && FindNextFile(hFind, &findData));

            FindClose(hFind);
        }
    }
}

void CDocument::AddNormalFile(LPCTSTR lpszFilePath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath)
{
    if (!*pcbAdding)
        return;

    if (!AddNormalFile(lpszFilePath))
        return;

    if (*lpszCurrentPath == 0)
        _tcsncpy_s(lpszCurrentPath, MAX_PATH, lpszFilePath, _TRUNCATE);
}

void CDocument::AddMd5File(LPCTSTR lpszMd5FilePath, const BOOL *pcbAdding, LPTSTR lpszCurrentPath)
{
    if (!*pcbAdding || (*lpszMd5FilePath == 0))
        return;

    if (*lpszCurrentPath == 0)
        _tcsncpy_s(lpszCurrentPath, MAX_PATH, lpszMd5FilePath, _TRUNCATE);

    CMd5File md5File;
    CMd5Item *pMd5Item;
    if (!md5File.Open(lpszMd5FilePath, CMd5File::ModeRead))
        return;
    while (*pcbAdding && md5File.Read(pMd5Item))
    {
        if (pMd5Item != NULL)
            AddMd5Item(pMd5Item);
    }
    md5File.Close();
}

void CDocument::AddMd5Item(CMd5Item *pMd5Item)
{
    AcquireWrite();

    int iCategory = GetCategory();
    CMd5Item *pMd5ItemOld = m_pMd5ItemList->GetMd5Item(pMd5Item->GetFullPath());
    if (pMd5ItemOld != NULL)
    {
        pMd5Item->MergeExisted(pMd5ItemOld);

        int i;
        if ((i = GetCache()->GetIndex(pMd5ItemOld, iCategory)) != -1)
            GetCache()->Delete(i, GetCount(), iCategory, FALSE);
        DeleteMd5Item(pMd5ItemOld);
    }

    InsertMd5Item(pMd5Item, FALSE);
    if ((iCategory == 0) || (pMd5Item->GetCategory() == iCategory))
        GetCache()->Insert(0, GetCount(), iCategory, TRUE);

    ReleaseWrite();
}

BOOL CDocument::LoadFromMd5File(LPCTSTR lpszFolderPath, LPCTSTR lpszMd5FileName, LPCTSTR lpszFullPath)
{
    if ((lpszFolderPath == NULL) || (*lpszFolderPath == 0) ||
        (lpszMd5FileName == NULL) || (*lpszMd5FileName == 0) ||
        (lpszFullPath == NULL) || (*lpszFullPath == 0))
        return FALSE;

    CPath md5FilePath(lpszFolderPath);
    if (!md5FilePath.Append(lpszMd5FileName))
        return FALSE;

    CMd5File md5File;
    CMd5Item *pMd5Item = NULL;
    if (!md5File.Open(md5FilePath.GetPath(), CMd5File::ModeRead))
        return FALSE;
    while (md5File.Read(pMd5Item))
    {
        if (pMd5Item != NULL)
        {
            if (_tcsicmp(pMd5Item->GetFullPath(), lpszFullPath) == 0)
            {
                AddMd5Item(pMd5Item);
                break;
            }
            else
            {
                delete pMd5Item;
                pMd5Item = NULL;
            }
        }
    }
    md5File.Close();

    return pMd5Item != NULL;
}

BOOL CDocument::GetFolderPath(LPCTSTR lpszFilePath, LPTSTR lpszFolderPath)
{
    if ((lpszFilePath == NULL) || (lpszFolderPath == NULL))
        return FALSE;

    _tcsncpy_s(lpszFolderPath, MAX_PATH, lpszFilePath, _TRUNCATE);
    if (!PathIsDirectory(lpszFolderPath))   // if the file path is invalid and happens to be a folder path,
        PathRemoveFileSpec(lpszFolderPath); // its MD5 should not be saved to its parent folder
    return PathIsDirectory(lpszFolderPath);
}

BOOL CDocument::GetMd5FilePath(LPCTSTR lpszFolderPath, LPTSTR lpszMd5FilePath)
{
    TCHAR szFileName[MAX_PATH];

    _tcsncpy_s(lpszMd5FilePath, MAX_PATH, lpszFolderPath, _TRUNCATE);
    if (g_app.GetOptions()->GetFolderName())
        CPathHelper::Cleanup(PathFindFileName(lpszMd5FilePath), szFileName);
    else
        CPathHelper::Cleanup(g_app.GetOptions()->GetDefaultName(), szFileName);
    if (!((*szFileName == 0) ? PathAppend(lpszMd5FilePath, MOD_DEFAULT_NAME) : PathAppend(lpszMd5FilePath, szFileName)))
        return FALSE;
    return _tcsncat_s(lpszMd5FilePath, MAX_PATH, MD5_EXTENSION, _TRUNCATE) == 0;
}

void CDocument::CleanUpMd5File(LPCTSTR lpszMd5FilePath)
{
    CMd5File md5File;
    CMd5Item *pMd5Item;
    MD5ITEMLIST listMd5Item;
    MD5ITEMLISTI iterMd5Item;

    if (!md5File.Open(lpszMd5FilePath, CMd5File::ModeRead))
        return;
    while (md5File.Read(pMd5Item))
    {
        if (pMd5Item != NULL)
        {
            if (m_pMd5ItemList->GetMd5Item(pMd5Item->GetFullPath()) == NULL)
                listMd5Item.push_back(pMd5Item);
            else
                delete pMd5Item;
        }
    }
    md5File.Close();

    md5File.Open(lpszMd5FilePath, CMd5File::ModeWrite);
    for (iterMd5Item = listMd5Item.begin(); iterMd5Item != listMd5Item.end(); iterMd5Item++)
    {
        md5File.Write(*iterMd5Item);
        delete *iterMd5Item;
    }
    listMd5Item.clear();
    md5File.Close();
}

void CDocument::OnGetDispInfo(LPNMLVDISPINFO lpnm)
{
    LPLVITEM lpLVItem = &lpnm->item;

    AcquireWrite();

    if (lpLVItem->iItem >= m_nCounts[m_iCategory])
    {
        ReleaseWrite();
        return;
    }

    CMd5ItemCached *pMd5ItemCached = m_pMd5ItemCache->GetMd5ItemCached(lpLVItem->iItem, m_nCounts[m_iCategory], m_iCategory);
    if (pMd5ItemCached == NULL)
    {
        ReleaseWrite();
        return;
    }
    CMd5Item *pMd5Item = pMd5ItemCached->GetMd5Item();
    if ((m_iCategory != 0) && (pMd5Item->GetCategory() != m_iCategory))
    {
        ReleaseWrite();
        return;
    }

    if ((lpLVItem->mask & LVIF_IMAGE) == LVIF_IMAGE)
        lpLVItem->iImage = pMd5Item->GetIcon();

    if ((lpLVItem->mask & LVIF_TEXT) == LVIF_TEXT)
    {
        switch (lpLVItem->iSubItem)
        {
        case 0:
            lpLVItem->pszText = (LPTSTR)pMd5Item->GetFileName();
            break;
        case 1:
            lpLVItem->pszText = (LPTSTR)pMd5ItemCached->GetFolderPath();
            break;
        case 2:
            lpLVItem->pszText = (LPTSTR)pMd5Item->GetCurrentMD5();
            break;
        case 3:
            lpLVItem->pszText = (LPTSTR)pMd5Item->GetSavedMD5();
            break;
        case 4:
            lpLVItem->pszText = (LPTSTR)pMd5ItemCached->GetSize();
            break;
        case 5:
            lpLVItem->pszText = (LPTSTR)pMd5ItemCached->GetModified();
            break;
        }
    }

    ReleaseWrite();
}

void CDocument::OnODCacheHint(LPNMLVCACHEHINT lpnm)
{
    AcquireWrite();
    m_pMd5ItemCache->Cache(lpnm->iFrom, lpnm->iTo, m_nCounts[m_iCategory], m_iCategory);
    ReleaseWrite();
}
