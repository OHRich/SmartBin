#include <windows.h>
#include "tray.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT  1001
#define ID_TRAY_OPEN  1002

HINSTANCE g_hInst;
HWND g_hWnd;

void ShowTrayMenu(HWND hWnd)
{
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_OPEN, L"Open Recycle Bin");
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
    DestroyMenu(hMenu);
}

void OpenRecycleBin()
{
    ShellExecute(nullptr, L"open", L"shell:RecycleBinFolder", nullptr, nullptr, SW_SHOWNORMAL);
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
        TrayCleanup();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    g_hInst = hInstance;

    const wchar_t CLASS_NAME[] = L"SmartBinTrayWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    g_hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"SmartBin",
        0,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hWnd)
        return 0;

    TrayInit(hInstance, g_hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
