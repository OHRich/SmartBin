#include "tray.h"
#include "resource.h"
#include <shellapi.h>

static NOTIFYICONDATA nid = {};

void TrayInit(HINSTANCE hInst, HWND hWnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(ICON_EMPTY));
    wcscpy_s(nid.szTip, L"SmartBin: Recycle Bin");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void TrayCleanup()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void UpdateTrayIcon(HINSTANCE hInst, bool isFull)
{
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(isFull ? ICON_FULL : ICON_EMPTY));
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}
