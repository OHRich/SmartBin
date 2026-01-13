#pragma once
#include <windows.h>

void TrayInit(HINSTANCE hInst, HWND hWnd);
void TrayCleanup();

void TrayUpdateFromSystem();
void TrayUpdateTooltip();
