#include "stdafx.h"
#include "WaitCursor.h"

LONG CWaitCursor::m_nWaitCursorCount = 0;
HCURSOR CWaitCursor::m_hcurWaitCursorRestore = NULL;

CWaitCursor::CWaitCursor()
{
    if (m_nWaitCursorCount == 0)
        m_hcurWaitCursorRestore = SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));
    InterlockedIncrementRelease(&m_nWaitCursorCount);
}

CWaitCursor::~CWaitCursor()
{
    InterlockedDecrementAcquire(&m_nWaitCursorCount);
    if (m_nWaitCursorCount == 0)
        SetCursor(m_hcurWaitCursorRestore);
}
