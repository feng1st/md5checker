#include "stdafx.h"
#include "Md5ItemCache.h"

CMd5ItemCache::CMd5ItemCache(CMd5ItemList *pMd5ItemList)
{
    m_pMd5ItemList = pMd5ItemList;

    m_iInternal = 0;
    m_iExternal = 0;

    m_nSize = 0;
}

CMd5ItemCache::~CMd5ItemCache()
{
    Clear();
}

CMd5Item *CMd5ItemCache::GetMd5Item(int i, int nCount, int iCategory) const
{
    if ((i < 0) || (i >= nCount))
        return NULL;

    if ((i >= m_iExternal) && (i < (m_iExternal + m_nSize)))
        return m_md5ItemCached[(m_iInternal + (i - m_iExternal)) % MAX_MD5_CACHE].GetMd5Item();
    else if ((m_nSize == 0) || (i <= (m_iExternal >> 1)))
        return m_pMd5ItemList->GetMd5Item(i, 0, m_pMd5ItemList->GetFirstIter(), iCategory);
    else if (i >= ((nCount + (m_iExternal + m_nSize)) >> 1))
        return m_pMd5ItemList->GetMd5Item(i, nCount - 1, m_pMd5ItemList->GetLastIter(), iCategory);
    else if (i < m_iExternal)
        return m_pMd5ItemList->GetMd5Item(i, m_iExternal - 1, m_pMd5ItemList->GetRIter(m_md5ItemCached[m_iInternal].GetMd5Item())++, iCategory);
    else
        return m_pMd5ItemList->GetMd5Item(i, m_iExternal + m_nSize, m_pMd5ItemList->GetIter(m_md5ItemCached[(m_iInternal + m_nSize - 1) % MAX_MD5_CACHE].GetMd5Item())++, iCategory);
}

int CMd5ItemCache::GetIndex(const CMd5Item *pMd5Item, int iCategory) const // iCategory can be CACHED_ONLY_CATEGORY
{
    for (int i = 0; i < m_nSize; i++)
    {
        if (m_md5ItemCached[(m_iInternal + i) % MAX_MD5_CACHE].GetMd5Item() == pMd5Item)
            return m_iExternal + i;
    }

    if (iCategory != CACHED_ONLY_CATEGORY)
        return m_pMd5ItemList->GetIndex(pMd5Item, iCategory);

    return -1;
}

BOOL CMd5ItemCache::Cache(int i, int nCount, int iCategory)
{
    if ((i < 0) || (i >= nCount))
        return FALSE;

    if ((i >= m_iExternal) && (i < (m_iExternal + m_nSize)))
        return TRUE;

    CMd5ItemList::Iter iter;
    CMd5ItemList::RIter rIter;
    CMd5Item *pMd5Item;

    if ((m_nSize == 0) || ((i < (m_iExternal - MAX_MD5_CACHE)) && (i < MAX_MD5_CACHE)))
    {
        m_iInternal = 0;
        m_iExternal = 0;
        m_nSize = 0;

        iter = m_pMd5ItemList->GetFirstIter();
        do
        {
            pMd5Item = m_pMd5ItemList->GetNext(iter);
        } while ((iCategory != 0) && (pMd5Item != NULL) && (pMd5Item->GetCategory() != iCategory));

        if (pMd5Item == NULL)
            return FALSE;

        m_md5ItemCached[m_iInternal].SetMd5Item(pMd5Item);
        m_nSize = 1;
    }
    else if ((i > (m_iExternal + m_nSize + MAX_MD5_CACHE)) && (i > (nCount - MAX_MD5_CACHE)))
    {
        m_iInternal = 0;
        m_iExternal = nCount - 1;
        m_nSize = 0;

        rIter = m_pMd5ItemList->GetLastIter();
        do
        {
            pMd5Item = m_pMd5ItemList->GetPrev(rIter);
        } while ((iCategory != 0) && (pMd5Item != NULL) && (pMd5Item->GetCategory() != iCategory));

        if (pMd5Item == NULL)
            return FALSE;

        m_md5ItemCached[m_iInternal].SetMd5Item(pMd5Item);
        m_nSize = 1;
    }

    if (i < m_iExternal)
    {
        rIter = m_pMd5ItemList->GetRIter(m_md5ItemCached[m_iInternal].GetMd5Item());
        m_pMd5ItemList->GetPrev(rIter);
        while (m_iExternal > i)
        {
            do
            {
                pMd5Item = m_pMd5ItemList->GetPrev(rIter);
            } while ((iCategory != 0) && (pMd5Item != NULL) && (pMd5Item->GetCategory() != iCategory));

            if (pMd5Item == NULL)
                return FALSE;

            m_iInternal = (m_iInternal + MAX_MD5_CACHE - 1) % MAX_MD5_CACHE;
            m_iExternal--;
            m_md5ItemCached[m_iInternal].SetMd5Item(pMd5Item);
            if (m_nSize < MAX_MD5_CACHE)
                m_nSize++;
        }
    }
    else if (i >= (m_iExternal + m_nSize))
    {
        iter = m_pMd5ItemList->GetIter(m_md5ItemCached[(m_iInternal + m_nSize - 1) % MAX_MD5_CACHE].GetMd5Item());
        m_pMd5ItemList->GetNext(iter);
        while (m_iExternal + m_nSize <= i)
        {
            do
            {
                pMd5Item = m_pMd5ItemList->GetNext(iter);
            } while ((iCategory != 0) && (pMd5Item != NULL) && (pMd5Item->GetCategory() != iCategory));

            if (pMd5Item == NULL)
                return FALSE;

            m_md5ItemCached[(m_iInternal + m_nSize) % MAX_MD5_CACHE].SetMd5Item(pMd5Item);
            if (m_nSize < MAX_MD5_CACHE)
            {
                m_nSize++;
            }
            else
            {
                m_iInternal = (m_iInternal + 1) % MAX_MD5_CACHE;
                m_iExternal++;
            }
        }
    }

    return TRUE;
}

// Do NOT recache before actual list item is deleted!
void CMd5ItemCache::Delete(int i, int nCount, int iCategory, BOOL bRecache)
{
    if (m_nSize == 0)
        return;

    if (i < m_iExternal)
    {
        m_iExternal--;
    }
    else if (i < (m_iExternal + (m_nSize >> 1)))
    {
        m_nSize -= i - m_iExternal + 1;
        m_iInternal = (m_iInternal + (i - m_iExternal + 1)) % MAX_MD5_CACHE;
        m_iExternal = i;

        if (bRecache)
        {
            if (m_iExternal > 0)
                Cache(m_iExternal - 1, nCount, iCategory);
            else
                Cache(m_iExternal + m_nSize, nCount, iCategory);
        }
    }
    else if (i < (m_iExternal + m_nSize))
    {
        m_nSize = i - m_iExternal;

        if (bRecache)
        {
            if (m_iExternal + m_nSize < nCount)
                Cache(m_iExternal + m_nSize, nCount, iCategory);
            else
                Cache(m_iExternal - 1, nCount, iCategory);
        }
    }
}

// Do NOT recache before actual list item is inserted!
void CMd5ItemCache::Insert(int i, int nCount, int iCategory, BOOL bRecache)
{
    if (m_nSize == 0)
        return;

    if (i <= m_iExternal)
    {
        m_iExternal++;
    }
    else if (i <= (m_iExternal + (m_nSize >> 1)))
    {
        m_nSize -= i - m_iExternal;
        m_iInternal = (m_iInternal + (i - m_iExternal)) % MAX_MD5_CACHE;
        m_iExternal = i + 1;

        if (bRecache)
            Cache(m_iExternal - 1, nCount, iCategory);
    }
    else if (i <= (m_iExternal + m_nSize))
    {
        m_nSize = i - m_iExternal;

        if (bRecache)
            Cache(m_iExternal + m_nSize, nCount, iCategory);
    }
}
