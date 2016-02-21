#include "stdafx.h"
#include "WindowHelper.h"

void CWindowHelper::CenterWindow(HWND hWnd)
{
    RECT rect, rectParent;

    GetWindowRect(hWnd, &rect);
    GetWindowRect(GetParent(hWnd), &rectParent);
    SetWindowPos(hWnd, NULL, (rectParent.right + rectParent.left - rect.right + rect.left) >> 1, (rectParent.bottom + rectParent.top - rect.bottom + rect.top) >> 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
