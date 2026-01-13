#include "tray.h"
#include "resource.h"
#include <shellapi.h>
#include <strsafe.h>

static NOTIFYICONDATA nid = {};
static HICON hIconEmpty = nullptr;
static HICON hIconFull = nullptr;

static bool      g_isFull = false;
static ULONGLONG g_size = 0;
static DWORD     g_count = 0;

HICON GetRecycleBinIcon(bool isFull)
{
    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);

    SHGetStockIconInfo(
        isFull ? SIID_RECYCLERFULL : SIID_RECYCLER,
        SHGSI_ICON | SHGSI_SMALLICON, // или SHGSI_LARGEICON
        &sii
    );

    return sii.hIcon;
}

void TrayInit(HINSTANCE hInst, HWND hWnd)
{
    hIconEmpty = (HICON)LoadImage(
        hInst,
        MAKEINTRESOURCE(ICON_EMPTY),
        IMAGE_ICON,
        16,
        16,
        LR_DEFAULTCOLOR
    );
    
    hIconFull = (HICON)LoadImage(
        hInst,
        MAKEINTRESOURCE(ICON_FULL),
        IMAGE_ICON,
        16,
        16,
        LR_DEFAULTCOLOR
    );

	// TODO Alternative way to get system icons: (don't forget to delete)
    //if (hIconEmpty)
    //{
    //    DestroyIcon(hIconEmpty);
    //    hIconEmpty = nullptr;
    //}
	//hIconEmpty = GetRecycleBinIcon(false);
	//hIconFull = GetRecycleBinIcon(true);

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = hIconEmpty;

    wcscpy_s(nid.szTip, L"SmartBin");

    Shell_NotifyIcon(NIM_ADD, &nid);

    TrayUpdateFromSystem();
}

void TrayCleanup()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);

    if (hIconEmpty)
    {
        DestroyIcon(hIconEmpty);
        hIconEmpty = nullptr;
    }

    if (hIconFull)
    {
        DestroyIcon(hIconFull);
        hIconFull = nullptr;
    }
}

void TrayUpdateFromSystem()
{
    SHQUERYRBINFO info = {};
    info.cbSize = sizeof(info);

    if (FAILED(SHQueryRecycleBin(nullptr, &info)))
        return;

    g_count = (DWORD)info.i64NumItems;
    g_size = info.i64Size;

    bool newIsFull = g_count > 0;
    if (newIsFull != g_isFull)
    {
        g_isFull = newIsFull;
        nid.hIcon = g_isFull ? hIconFull : hIconEmpty;
        Shell_NotifyIcon(NIM_MODIFY, &nid);
    }

    TrayUpdateTooltip();
}

void TrayUpdateTooltip()
{
    wchar_t tip[128];

    if (g_count == 0)
    {
        wcscpy_s(tip, L"SmartBin\nRecycle Bin is empty");
    }
    else
    {
        double mb = g_size / (1024.0 * 1024.0);
        StringCchPrintf(
            tip, 128,
            L"SmartBin\nFiles: %u\nSize: %.2f MB",
            g_count, mb
        );
    }

    wcscpy_s(nid.szTip, tip);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}
