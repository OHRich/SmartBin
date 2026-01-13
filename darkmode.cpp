#include "darkmode.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

enum PreferredAppMode
{
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
};

using fnSetPreferredAppMode = PreferredAppMode(WINAPI*)(PreferredAppMode);
using fnFlushMenuThemes = void(WINAPI*)();

static fnSetPreferredAppMode SetPreferredAppMode = nullptr;
static fnFlushMenuThemes FlushMenuThemes = nullptr;

bool IsDarkModeEnabled()
{
    HKEY hKey;
    DWORD value = 1, size = sizeof(DWORD);

    if (RegOpenKeyEx(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0,
        KEY_READ,
        &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueEx(hKey, L"AppsUseLightTheme", nullptr, nullptr, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }
    return value == 0;
}

void InitDarkMode()
{
    HMODULE hUxtheme = LoadLibrary(L"uxtheme.dll");
    if (!hUxtheme) return;

    SetPreferredAppMode =
        (fnSetPreferredAppMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    FlushMenuThemes =
        (fnFlushMenuThemes)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136));

    if (SetPreferredAppMode)
        SetPreferredAppMode(AllowDark);

    if (FlushMenuThemes)
        FlushMenuThemes();
}
