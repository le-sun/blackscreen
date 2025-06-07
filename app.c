#include <windows.h>
#include <shellapi.h>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_TOGGLE 1002
#define HOTKEY_ID 1

HWND g_hwnd = NULL;
HINSTANCE g_hInstance = NULL;
NOTIFYICONDATA nid = {0};
BOOL g_isBlackScreenVisible = FALSE;
HMENU g_hTrayMenu = NULL;  // Reusable menu

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            // Register global hotkey (F14)
            RegisterHotKey(hwnd, HOTKEY_ID, 0, VK_F14);
            
            // Setup system tray icon with fallback
            nid.cbSize = sizeof(NOTIFYICONDATA);
            nid.hWnd = hwnd;
            nid.uID = 1;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON;
            
            // Try custom icon first, fallback to system icon
            nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MYICON));
            if (!nid.hIcon) {
                nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            }
            
            strcpy(nid.szTip, "Black Screen - Press F14");
            Shell_NotifyIcon(NIM_ADD, &nid);
            
            // Create reusable context menu
            g_hTrayMenu = CreatePopupMenu();
            AppendMenu(g_hTrayMenu, MF_STRING, ID_TRAY_TOGGLE, "Toggle Black Screen");
            AppendMenu(g_hTrayMenu, MF_STRING, ID_TRAY_EXIT, "Exit");
            break;
            
        case WM_HOTKEY:
            if (wParam == HOTKEY_ID) {
                // Toggle black screen visibility
                if (g_isBlackScreenVisible) {
                    ShowWindow(hwnd, SW_HIDE);
                    g_isBlackScreenVisible = FALSE;
                } else {
                    ShowWindow(hwnd, SW_SHOW);
                    SetForegroundWindow(hwnd);
                    g_isBlackScreenVisible = TRUE;
                }
            }
            break;
            
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP) {
                // Show context menu on right-click
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(g_hTrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            }
            break;
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_TRAY_TOGGLE:
                    // Same as hotkey - toggle visibility
                    if (g_isBlackScreenVisible) {
                        ShowWindow(hwnd, SW_HIDE);
                        g_isBlackScreenVisible = FALSE;
                    } else {
                        ShowWindow(hwnd, SW_SHOW);
                        SetForegroundWindow(hwnd);
                        g_isBlackScreenVisible = TRUE;
                    }
                    break;
                case ID_TRAY_EXIT:
                    PostQuitMessage(0);
                    break;
            }
            break;
            
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE && g_isBlackScreenVisible) {
                ShowWindow(hwnd, SW_HIDE);
                g_isBlackScreenVisible = FALSE;
            }
            break;
            
        case WM_DESTROY:
            // Cleanup
            UnregisterHotKey(hwnd, HOTKEY_ID);
            Shell_NotifyIcon(NIM_DELETE, &nid);
            if (g_hTrayMenu) {
                DestroyMenu(g_hTrayMenu);
                g_hTrayMenu = NULL;
            }
            // Clean up icon if it was loaded from resources
            if (nid.hIcon) {
                DestroyIcon(nid.hIcon);
                nid.hIcon = NULL;
            }
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    g_hInstance = hInstance;  // Store for global use
    const char CLASS_NAME[] = "BlackScreenClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // Performance optimization

    RegisterClass(&wc);

    g_hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        CLASS_NAME,
        "Black Screen",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL
    );

    MSG msg = {0};
    while (TRUE) {
        // Use PeekMessage to avoid blocking and reduce CPU usage
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // Wait for messages instead of polling - more CPU efficient
            WaitMessage();
        }
    }

    return 0;
}
