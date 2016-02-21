#include "stdafx.h"
#include "Md5ItemCached.h"

void CMd5ItemCached::SetMd5Item(CMd5Item *pMd5Item)
{
    if ((pMd5Item != m_pMd5Item) || (pMd5Item->GetVersion() != m_nVersion))
    {
        m_pMd5Item = pMd5Item;
        m_nVersion = pMd5Item->GetVersion();

        if (pMd5Item->GetFileName() == pMd5Item->GetFullPath())
            *m_szFolderPath = 0;
        else
        {
            _tcsncpy_s(m_szFolderPath, MAX_PATH, pMd5Item->GetFullPath(), _TRUNCATE);
            PathRemoveFileSpec(m_szFolderPath);
        }

        if (!pMd5Item->IsNA())
        {
            TCHAR szSize[MAX_NUM_STR];
            NUMBERFMT nf;
            nf.NumDigits = 0;
            nf.LeadingZero = 0;
            nf.Grouping = 3;
            nf.lpDecimalSep = _T(".");
            nf.lpThousandSep = _T(",");
            nf.NegativeOrder = 1;
            _stprintf_s(szSize, MAX_NUM_STR, _T("%lld"), pMd5Item->GetSize());
            GetNumberFormat(LOCALE_USER_DEFAULT, 0, szSize, &nf, m_szSize, MAX_NUM_STR);

            TCHAR szDate[MAX_DATE_STR];
            TCHAR szTime[MAX_DATE_STR];
            SYSTEMTIME st;
            FileTimeToSystemTime(&pMd5Item->GetModified(), &st);
            GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, szDate, MAX_DATE_STR);
            GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szTime, MAX_DATE_STR);
            _stprintf_s(m_szModified, MAX_DATE_STR, _T("%s %s"), szDate, szTime);
        }
        else
        {
            *m_szSize = 0;
            *m_szModified = 0;
        }
    }
}
