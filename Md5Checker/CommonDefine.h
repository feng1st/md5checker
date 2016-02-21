#pragma once

#define MAX_INT_STR         16
#define MAX_NUM_STR         32
#define MAX_DATE_STR        64

#define MAX_CAPTION         256
#define MAX_TEXT            512
#define MAX_EDIT_TEXT       32768

#define MAX_PATHS       32768

#define HEX_TABLE       _T("0123456789ABCDEF")

#define WM_USER_SELECTION_CHANGED       (WM_USER + 1)
#define WM_USER_MD5_ITEMS_DELETED       (WM_USER + 2)
#define WM_USER_ADD_CANCELLED           (WM_USER + 3)
#define WM_USER_ADD_STARTED             (WM_USER + 4)
#define WM_USER_ADD_STOPPED             (WM_USER + 5)
#define WM_USER_CHECK_CANCELLED         (WM_USER + 6)
#define WM_USER_CHECK_STARTED           (WM_USER + 7)
#define WM_USER_CHECK_STOPPED           (WM_USER + 8)
#define WM_USER_PROGRESS_BAR_SHOW       (WM_USER + 9)
#define WM_USER_UPDATE_VIEW_TOOLBAR     (WM_USER + 10)
#define WM_USER_SET_CURRENT_MD5         (WM_USER + 11)
#define WM_USER_MARK_AS_NA              (WM_USER + 12)
#define WM_USER_COMPARE_MD5             (WM_USER + 13)
#define WM_USER_UPDATE_TITLE            (WM_USER + 14)

#define TIMER_ADDING_ID         1
#define TIMER_CHECKING_ID       2
