#include <shlobj.h>
#include "tray.h"
#include "darkmode.h"

#define WM_TRAYICON      (WM_USER + 1)
#define WM_SHELL_NOTIFY  (WM_USER + 100)
#define ID_DEBOUNCE      1

#define ID_TRAY_EXIT  1001
#define ID_TRAY_OPEN  1002

static ULONG g_shellNotifyId = 0;

void ShowTrayMenu(HWND hWnd)
{
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_OPEN, L"Open Recycle Bin");
    AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
    DestroyMenu(hMenu);
}

void OpenRecycleBin()
{
    ShellExecute(nullptr, L"open", L"shell:RecycleBinFolder", nullptr, nullptr, SW_SHOWNORMAL);
}

void RegisterShellNotify(HWND hWnd)
{
    SHChangeNotifyEntry entry = {};
    entry.pidl = nullptr;
    entry.fRecursive = TRUE;

    g_shellNotifyId = SHChangeNotifyRegister(
        hWnd,
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel,
        SHCNE_CREATE | SHCNE_DELETE | SHCNE_UPDATEDIR |
        SHCNE_MKDIR | SHCNE_RMDIR,
        WM_SHELL_NOTIFY,
        1,
        &entry
    );
}

void UnregisterShellNotify()
{
    if (g_shellNotifyId)
        SHChangeNotifyDeregister(g_shellNotifyId);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TRAYICON:
        switch (lParam)
        {
        case WM_RBUTTONUP:
            ShowTrayMenu(hWnd);
            break;
        case WM_LBUTTONDBLCLK:
            OpenRecycleBin();
            break;
        }
        break;

    case WM_SHELL_NOTIFY:
        KillTimer(hWnd, ID_DEBOUNCE);
        SetTimer(hWnd, ID_DEBOUNCE, 300, nullptr);
        break;

    case WM_TIMER:
        if (wParam == ID_DEBOUNCE)
        {
            KillTimer(hWnd, ID_DEBOUNCE);
            TrayUpdateFromSystem();
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_TRAY_OPEN:
            OpenRecycleBin();
            break;
        case ID_TRAY_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        UnregisterShellNotify();
        TrayCleanup();
        PostQuitMessage(0);
        // TODO Kill the timer if it's not running but the app is already terminating.
        //KillTimer(hWnd, 1);
        //TrayCleanup();
        //PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    InitDarkMode();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SmartBinWindow";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0, wc.lpszClassName, L"SmartBin",
        0, 0, 0, 0, 0,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd)
        return 0;

    TrayInit(hInstance, hWnd);
    RegisterShellNotify(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
