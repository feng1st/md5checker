#include "stdafx.h"
#include "Md5ItemList.h"

CMd5ItemList::CMd5ItemList()
{
    m_pList = new List();
    m_pMap = new Map();
}

CMd5ItemList::~CMd5ItemList()
{
    Clear();

    if (m_pMap != NULL)
    {
        delete m_pMap;
        m_pMap = NULL;
    }
    if (m_pList != NULL)
    {
        delete m_pList;
        m_pList = NULL;
    }
}

CMd5Item *CMd5ItemList::GetMd5Item(int i, int iStart, Iter iterStart, int iCategory) const
{
    CMd5Item *pMd5ItemCurr;
    int iCurr = iStart;
    Iter iter = iterStart;
    while ((pMd5ItemCurr = GetNext(iter)) != NULL)
    {
        if ((iCategory == 0) || (pMd5ItemCurr->GetCategory() == iCategory))
        {
            if (iCurr++ == i)
                return pMd5ItemCurr;
        }
    }

    return NULL;
}

CMd5Item *CMd5ItemList::GetMd5Item(int i, int iStart, RIter rIterStart, int iCategory) const
{
    CMd5Item *pMd5ItemCurr;
    int iCurr = iStart;
    RIter rIter = rIterStart;
    while ((pMd5ItemCurr = GetPrev(rIter)) != NULL)
    {
        if ((iCategory == 0) || (pMd5ItemCurr->GetCategory() == iCategory))
        {
            if (iCurr-- == i)
                return pMd5ItemCurr;
        }
    }

    return NULL;
}

int CMd5ItemList::GetIndex(const CMd5Item *pMd5Item, int iCategory) const
{
    if ((iCategory != 0) && (pMd5Item->GetCategory() != iCategory))
        return -1;

    int i = 0;
    CMd5Item *pMd5ItemCurr;
    Iter iter = GetFirstIter();
    while ((pMd5ItemCurr = GetNext(iter)) != NULL)
    {
        if (pMd5ItemCurr == pMd5Item)
            return i;

        if ((iCategory == 0) || (pMd5ItemCurr->GetCategory() == iCategory))
            i++;
    }

    return -1;
}

void CMd5ItemList::Clear()
{
    m_pMap->clear();

    CMd5Item *pMd5Item;
    Iter iter = GetFirstIter();
    while ((pMd5Item = GetNext(iter)) != NULL)
        delete pMd5Item;
    m_pList->clear();
}
