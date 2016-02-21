#pragma once

#include "AddThread.h"
#include "CheckThread.h"
#include "Document.h"
#include "MainWindow.h"
#include "CompareMD5Dialog.h"
#include "Options.h"

class CMd5Checker
{
public:
    CMd5Checker();
    virtual ~CMd5Checker();

public:

    virtual HWND GetMainWnd() const
    {
        return m_pMainWindow->GetWnd();
    }

    virtual COptions *GetOptions() const
    {
        return m_pOptions;
    }

    virtual CDocument *GetDocument() const
    {
        return m_pDocument;
    }

    virtual CMainWindow *GetMainWindow() const
    {
        return m_pMainWindow;
    }

    virtual CCompareMD5Dialog *GetCompareMD5Dialog() const
    {
        return m_pCompareMD5Dialog;
    }

public:

    virtual BOOL PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        return GetMainWindow()->PostMessage(Msg, wParam, lParam);
    }

    virtual LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        return GetMainWindow()->SendMessage(Msg, wParam, lParam);
    }

    virtual LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL Init();
    virtual void ParseCommandLine(BOOL &rbMixed);
    virtual int Run();

public:
    virtual void OnFileSave();
    virtual void OnFileSaveEach();
    virtual void OnFileSaveTo();
    virtual void OnEditAdd();
    virtual void OnEditClear();
    virtual void OnEditStart();
    virtual void OnEditStop();
    virtual void OnView(UINT uID);
    virtual void OnToolsSelfCheck();
    virtual void OnToolsMD5Calculator();
    virtual void OnToolsOptions();
    virtual void OnOnlineHelp();
    virtual void OnHelpCheckForUpdates();
    virtual void OnHelpAbout();
    virtual void OnDropFiles(HDROP hDrop);
    virtual void OnInitMenuPopup(HMENU hPopupMenu, UINT nIndex, BOOL bSysMenu);
    virtual void OnColumnClick(LPNMLISTVIEW lpnm);
    virtual void OnTimerAdding();
    virtual void OnTimerChecking();
    virtual void OnAddCancelled();
    virtual void OnAddStarted();
    virtual void OnAddStopped();
    virtual void OnCheckCancelled();
    virtual void OnCheckStarted();
    virtual void OnCheckStopped();
    virtual void OnMd5ItemsDeleted();

protected:
    COptions *m_pOptions;
    CDocument *m_pDocument;
    CMainWindow *m_pMainWindow;
    CAddThread *m_pAddThread;
    CCheckThread *m_pCheckThread;
    CCompareMD5Dialog *m_pCompareMD5Dialog;

    int m_iSort;
    BOOL m_bAsc;
};

extern HINSTANCE g_hInstance;
extern CMd5Checker g_app;
