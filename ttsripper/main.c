#define Rectangle rlRectangle
#define CloseWindow rlCloseWindow
#define ShowCursor rlShowCursor

#include "raylib.h"

#undef Rectangle
#undef CloseWindow
#undef ShowCursor

#define LoadImage WinLoadImage  // only if needed

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>

#undef LoadImage

#include <stdio.h>
#include "split.c"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "raylib.lib")

#define ID_BTN_BROWSE 1001
#define ID_EDIT_PATH  1002
#define ID_SPIN1      1003
#define ID_SPIN2      1004
#define ID_EDIT1      1005
#define ID_EDIT2      1006
#define ID_BTN_SUBMIT 1007

char filePath[MAX_PATH] = "";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // File path edit box
        CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            10, 10, 260, 25, hwnd, (HMENU)ID_EDIT_PATH, NULL, NULL);

        // Browse button
        CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE,
            280, 10, 80, 25, hwnd, (HMENU)ID_BTN_BROWSE, NULL, NULL);

        // Number 1 label + edit + spin
        CreateWindow("STATIC", "Width", WS_CHILD | WS_VISIBLE,
            10, 50, 70, 20, hwnd, NULL, NULL, NULL);
        HWND hEdit1 = CreateWindow("EDIT", "0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            80, 50, 50, 20, hwnd, (HMENU)ID_EDIT1, NULL, NULL);
        HWND hSpin1 = CreateWindow(UPDOWN_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS,
            130, 50, 20, 20, hwnd, (HMENU)ID_SPIN1, NULL, NULL);
        SendMessage(hSpin1, UDM_SETBUDDY, (WPARAM)hEdit1, 0);
        SendMessage(hSpin1, UDM_SETRANGE, 0, MAKELPARAM(100, 0));

        // Number 2 label + edit + spin
        CreateWindow("STATIC", "Height", WS_CHILD | WS_VISIBLE,
            10, 80, 70, 20, hwnd, NULL, NULL, NULL);
        HWND hEdit2 = CreateWindow("EDIT", "0", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            80, 80, 50, 20, hwnd, (HMENU)ID_EDIT2, NULL, NULL);
        HWND hSpin2 = CreateWindow(UPDOWN_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS,
            130, 80, 20, 20, hwnd, (HMENU)ID_SPIN2, NULL, NULL);
        SendMessage(hSpin2, UDM_SETBUDDY, (WPARAM)hEdit2, 0);
        SendMessage(hSpin2, UDM_SETRANGE, 0, MAKELPARAM(100, 0));

        // Submit button
        CreateWindow("BUTTON", "Let it rip!", WS_CHILD | WS_VISIBLE,
            10, 120, 100, 30, hwnd, (HMENU)ID_BTN_SUBMIT, NULL, NULL);
        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_BTN_BROWSE: {
            OPENFILENAME ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = "All Files\0*.*\0";
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                SetDlgItemText(hwnd, ID_EDIT_PATH, filePath);
            }
            break;
        }
        case ID_BTN_SUBMIT: {
            char number1[16], number2[16];
            GetDlgItemText(hwnd, ID_EDIT1, number1, sizeof(number1));
            GetDlgItemText(hwnd, ID_EDIT2, number2, sizeof(number2));

            int width  = toint(number1, sizeof(number1));
            int height = toint(number2, sizeof(number2));
            int count = split(filePath, width, height);

            char message[512];
            snprintf(message, sizeof(message), "Created %d files.", count);
            MessageBox(hwnd, message, "Submission", MB_OK);
            break;
        }
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    // BOOL dpi_result = SetProcessDPIAware();
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_UPDOWN_CLASS };
    InitCommonControlsEx(&icex);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyAppClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("MyAppClass", "TTSRipper",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 220,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
