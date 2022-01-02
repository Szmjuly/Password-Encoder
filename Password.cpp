// Password.cpp : Defines the entry point for the application
#include "framework.h"
#include "Password.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <wchar.h>

#define MAX_LOADSTRING 100

typedef unsigned int u32;

struct Render_State
{
    void* memory;
    int width, height;

    BITMAPINFO bitmap_info;
};

Render_State render_state;

#include "Rendering.cpp"
#include <string>

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND TextBox, SendButton, Output, Button, CypherLabel, PasswordListLabel, PasswordList;
HWND CypherInput = 0;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

bool running = true;
bool file_exists = false;

const char* filename = "D:/Programs/Password/Files/passwords.txt";

// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void encode();
void savePassword();
void displayPasswords(const char*);
void checkFile(const char*, bool&);
void fileExists(wchar_t*, int);
void fileDoesNotExist(wchar_t*, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    HWND window;
    MSG msg;
    HDC hdc;
    WNDCLASS wcex = {};

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PASSWORD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PASSWORD);
    wcex.lpszClassName  = L"Password Window Class";

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClass(&wcex))
        return 0;

    window = CreateWindowEx(0, 
                                wcex.lpszClassName,
                                _T("Passwords"),
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                1280, 720,
                                HWND_DESKTOP,        /* The window is a child-window to desktop */
                                0, hInstance, 0);

    hdc = GetDC(window);

    // Main message loop:
    while (running)
    {
        while (PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //// Rendering
        //{
        //    // Clear Screen
        //    clear_screen(0xf2f2f2);

        //    // Render
        //    StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY); // Renders a black screen
        //}
      }
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message)
    {   case WM_COMMAND: {  // Parse the menu selections:
                            switch (LOWORD(wParam))
                            {
                                case IDM_ABOUT: { DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About); } break;
                                case IDM_EXIT: { DestroyWindow(hWnd); } break;
                                case 1: {   encode(); } break;
                                case 2: {   SetWindowText(Output, L""); } break;
                                case 3: {   savePassword();  } break;
                                case 4: {   SetWindowText(CypherInput, L""); } break;
                                case 5: {   displayPasswords(filename); } break;
                                default: { return DefWindowProc(hWnd, message, wParam, lParam); }
                            }
                          } break;
        case WM_PAINT: {  PAINTSTRUCT ps;
                          HDC hdc = BeginPaint(hWnd, &ps);
                          // TODO: Add any drawing code that uses hdc here...
                          EndPaint(hWnd, &ps);
                        } break;
        case WM_CLOSE:
        case WM_DESTROY: { running = false;
                           //PostQuitMessage(0);
                          } break;
        case WM_SIZE: { RECT rect;

                        GetClientRect(hWnd, &rect);
                        render_state.width = rect.right - rect.left;
                        render_state.height = rect.bottom - rect.top;

                        int size = render_state.width * render_state.height * sizeof(u32);
                        if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
                        render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

                        render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
                        render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
                        render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
                        render_state.bitmap_info.bmiHeader.biPlanes = 1;
                        render_state.bitmap_info.bmiHeader.biBitCount = 32;
                        render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
                    } break;
        case WM_CREATE: {
            Button = CreateWindow(  L"BUTTON",  // Predefined class; Unicode assumed 
                                    L"Enter",      // Button text 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                                    10, 10, 100, 50,
                                    hWnd, (HMENU) 1, NULL, NULL);

            Button = CreateWindow(  L"BUTTON",  // Predefined class; Unicode assumed 
                                    L"Clear Output",      // Button text 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                                    150, 10, 100, 50,
                                    hWnd, (HMENU) 2, NULL, NULL);

            Button = CreateWindow(  L"BUTTON",  // Predefined class; Unicode assumed 
                                    L"Save Password",      // Button text 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                                    285, 10, 120, 50,
                                    hWnd, (HMENU) 3, NULL, NULL);

            Button = CreateWindow(  L"BUTTON",  // Predefined class; Unicode assumed 
                                    L"Clear Cypher",      // Button text 
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
                                    150, 70, 100, 20,
                                    hWnd, (HMENU) 4, NULL, NULL);

            CypherLabel = CreateWindow( L"STATIC",  // Predefined class; Unicode assumed 
                                        L"Enter Cypher:",      // Button text 
                                        WS_CHILD | WS_VISIBLE, // Styles 
                                        10, 70, 100, 20,
                                        hWnd, NULL, NULL, NULL);

            CypherInput = CreateWindow( L"EDIT",  // Predefined class; Unicode assumed 
                                        L"",      // Button text 
                                        WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE,  // Styles 
                                        123, 70, 12, 20,
                                        hWnd, NULL, NULL, NULL);

            TextBox = CreateWindow( L"EDIT",  // Predefined class; Unicode assumed 
                                    L"",      // Button text 
                                    WS_BORDER | WS_CHILD | WS_VISIBLE,  // Styles 
                                    10, 100, 395, 20,
                                    hWnd, NULL, NULL, NULL);

            
            Output = CreateWindow(  L"STATIC",  // Predefined class; Unicode assumed 
                                    L"",      // Button text 
                                    WS_BORDER | WS_CHILD | WS_VISIBLE | ES_READONLY,  // Styles 
                                    10, 150, 395, 20,
                                    hWnd, NULL, NULL, NULL);

            PasswordListLabel = CreateWindow(    L"STATIC",  // Predefined class; Unicode assumed 
                                            L"Passwords:",      // Button text 
                                            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_CENTER,  // Styles 
                                            420, 10, 395, 20,
                                            hWnd, NULL, NULL, NULL);

            PasswordList = CreateWindow(    L"EDIT",  // Predefined class; Unicode assumed 
                                            L"",      // Button text 
                                            WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,  // Styles 
                                            420, 40, 395, 420,
                                            hWnd, (HMENU) 5, NULL, NULL);
        }
        default: { result = DefWindowProc(hWnd, message, wParam, lParam); }
    }
    return result;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void encode() {
    int len = GetWindowTextLength(TextBox) + 1;
    int cypher_len = GetWindowTextLength(CypherInput) + 1;

    wchar_t* cypher = new wchar_t[cypher_len];
    wchar_t* text = new wchar_t[len];
    wchar_t* encoded_text = new wchar_t[len];

    GetWindowText(TextBox, &text[0], len);
    GetWindowText(Output, &encoded_text[0], len);
    GetWindowText(CypherInput, &cypher[0], cypher_len);

    int key = int(cypher[0]) - 48;

    if (key < 0) {
        key = 0;
    }

    for (int i = 0; i < len; ++i) {
        int val = int(text[i]);
        int duplicate = key;

        if (val + key > 122) {
            key -= (122 - val);
            key = key % 26;
            encoded_text[i] = char(96 + key);
        }
        else {
            encoded_text[i] = char(val + key);

            if (i == len - 1) {
                val = 0;
                encoded_text[i] = char(val);
            }
        }

        key = duplicate;
    }

    SetWindowText(Output, &encoded_text[0]);
    SetWindowText(TextBox, L"");

    delete[] text, cypher, encoded_text;
}

void savePassword() {
    int len = GetWindowTextLength(Output) + 1;
    wchar_t* password = new wchar_t[len];
    GetWindowText(Output, &password[0], len);
    

    checkFile(filename, file_exists);

    if (file_exists)
        fileExists(password, len);
    else
        fileDoesNotExist(password, len);
}

std::ifstream::pos_type filesize(const char* filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

void checkFile(const char* filename, bool &file_exists) {
    std::ifstream ifile;
    ifile.open(filename);

    if (ifile) {
        file_exists = true;
        ifile.close();
    }
    else {
        file_exists = false;
        ifile.close();
    }
}

void fileDoesNotExist(wchar_t* p, int p_length) {
    std::fstream passwords;
    passwords.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
    passwords << "Passwords:\r";
    passwords << "-----------------------------------------------\r";
    
    for (int i = 0; i < p_length; ++i) {
        int val = int(p[i]);
        char letter = char(val);
        passwords << letter;
    }
    passwords << std::endl;
    passwords.close();
}

void fileExists(wchar_t* p, int p_length) {
    std::fstream passwords;
    passwords.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);

    for (int i = 0; i < p_length; ++i) {
        int val = int(p[i]);
        char letter = char(val);

        passwords << letter;
    }
    passwords << std::endl;
    passwords.close();
}

void displayPasswords(const char* filename) {
    FILE* file;
    int _size = filesize(filename);
    wchar_t* data = new wchar_t[_size];
    GetWindowText(PasswordList, &data[0], _size);

    fopen_s(&file, filename, "r");

    if (file != NULL) {
        while (std::fgetws(data, _size, file)) {
            int outLength = GetWindowTextLength(PasswordList) + lstrlen(data) + 1;
            
            wchar_t* newText = new wchar_t[outLength];

            const wchar_t* line_end = L"\r\n";
            #pragma warning(suppress : 4996)
            wcscat(data, line_end);

            if (wcschr(data, '\n')) {
                // create buffer to hold current and new text
                wchar_t* buf = (wchar_t*)GlobalAlloc(GPTR, outLength * sizeof(wchar_t));
                if (!buf) return;

                // get existing text from edit control and put into buffer
                GetWindowText(PasswordList, buf, outLength);

                // append the newText to the buffer
                #pragma warning(suppress : 4996)
                wcscat(buf, data);

                // Set the text in the edit control
                SetWindowText(PasswordList, buf);

                // free the buffer
                GlobalFree(buf);
            }
        }
        
    }
    
}