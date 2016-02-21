#pragma once

class CWaitCursor
{
public:
    CWaitCursor();
    virtual ~CWaitCursor();

protected:
    static LONG m_nWaitCursorCount;
    static HCURSOR m_hcurWaitCursorRestore;
};
