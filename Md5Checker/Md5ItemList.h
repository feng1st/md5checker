#pragma once

#include <list>
#include <map>

#include "functionalext.h"
#include "Md5Item.h"

using namespace std;

class CMd5ItemList
{
public:
    typedef list<CMd5Item *> List;
    typedef List::iterator Iter;
    typedef List::reverse_iterator RIter;

protected:
    typedef map<LPCTSTR, Iter, _tcsiless<LPCTSTR>> Map;
    typedef Map::iterator Map_Iter;

public:
    CMd5ItemList();
    virtual ~CMd5ItemList();

public:

    virtual int GetSize() const
    {
        return (int)m_pList->size();
    }

    virtual Iter GetFirstIter() const
    {
        return m_pList->begin();
    }

    virtual RIter GetLastIter() const
    {
        return m_pList->rbegin();
    }

    virtual Iter GetIter(LPCTSTR lpszFullPath) const
    {
        Map_Iter map_iter = m_pMap->find(lpszFullPath);
        if (map_iter != m_pMap->end())
            return map_iter->second;
        else
            return m_pList->end();
    }

    virtual Iter GetIter(const CMd5Item *pMd5Item) const
    {
        return GetIter(pMd5Item->GetFullPath());
    }

    virtual RIter GetRIter(LPCTSTR lpszFullPath) const
    {
        Map_Iter map_iter = m_pMap->find(lpszFullPath);
        if (map_iter != m_pMap->end())
            return --(RIter)(map_iter->second);
        else
            return m_pList->rend();
    }

    virtual RIter GetRIter(const CMd5Item *pMd5Item) const
    {
        return GetRIter(pMd5Item->GetFullPath());
    }

    virtual CMd5Item *GetNext(Iter &rIter) const
    {
        if (rIter != m_pList->end())
            return *(rIter++);
        else
            return NULL;
    }

    virtual CMd5Item *GetPrev(RIter &rRIter) const
    {
        if (rRIter != m_pList->rend())
            return *(rRIter++);
        else
            return NULL;
    }

    virtual CMd5Item *GetFirstMd5Item() const
    {
        if (GetSize() > 0)
            return m_pList->front();
        else
            return NULL;
    }

    virtual CMd5Item *GetMd5Item(LPCTSTR lpszFullPath) const
    {
        Map_Iter map_iter = m_pMap->find(lpszFullPath);
        if (map_iter != m_pMap->end())
            return *(map_iter->second);
        else
            return NULL;
    }

    virtual CMd5Item *GetMd5Item(int i, int iStart, Iter iterStart, int iCategory) const;
    virtual CMd5Item *GetMd5Item(int i, int iStart, RIter rIterStart, int iCategory) const;
    virtual int GetIndex(const CMd5Item *pMd5Item, int iCategory) const;

public:

    virtual BOOL Insert(CMd5Item *pMd5Item, BOOL bCheckExist = TRUE)
    {
        if (bCheckExist)
        {
            Map_Iter map_iter = m_pMap->find(pMd5Item->GetFullPath());
            if (map_iter != m_pMap->end())
                return FALSE;
        }

        (*m_pMap)[pMd5Item->GetFullPath()] = m_pList->insert(m_pList->begin(), pMd5Item);

        return TRUE;
    }

    virtual BOOL Delete(CMd5Item *pMd5Item)
    {
        Map_Iter map_iter = m_pMap->find(pMd5Item->GetFullPath());
        if (map_iter != m_pMap->end())
        {
            delete pMd5Item;
            m_pList->erase(map_iter->second);
            m_pMap->erase(map_iter);

            return TRUE;
        }

        return FALSE;
    }

    virtual void MoveOut(Iter iter)
    {
        m_pList->erase(iter);
    }

    virtual void MoveIn(CMd5Item *pMd5Item)
    {
        m_pMap->find(pMd5Item->GetFullPath())->second = m_pList->insert(m_pList->begin(), pMd5Item);
    }

    virtual void Clear();

protected:
    List *m_pList;
    Map *m_pMap;
};
