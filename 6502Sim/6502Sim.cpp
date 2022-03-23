// 6502Sim.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "6502Sim.h"
#include "Processor.h" 

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//I'm going to make this a global variable for now, as I'm going to be initialize it in the wWinMain so that it is available to the 
Processor* emu_cpu = nullptr; //initialized to null ptr for safety

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ResetDialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TestDialog(HWND, UINT, WPARAM, LPARAM);

/*
   Controls Declarations
*/

//One thing I cannot seem to find much information on is where to declare controls that I'll be using and updating throughout the application, so I'm going to declare my controls globally for now
HWND a_reg_readout;
HWND x_reg_readout;
HWND y_reg_readout;
HWND sp_reg_readout;
HWND f_n_readout;
HWND f_v_readout;
HWND f_b_readout;
HWND f_d_readout;
HWND f_i_readout;
HWND f_z_readout;
HWND f_c_readout;
HWND f_n_label;
HWND f_v_label;
HWND f_b_label;
HWND f_d_label;
HWND f_i_label;
HWND f_z_label;
HWND f_c_label;
HWND output_readout;
HWND pch_readout;
HWND pcl_readout;


//text boxes for viewing ram/rom, I think these will be textboxes of some sorts, with autoscroll or some other formatting done
HWND ram_viewer;
HWND rom_viewer;

//the button control for 
HWND step_button;

//labels for everything
HWND a_label;
HWND x_label;
HWND y_label;
HWND pch_label;
HWND pcl_label;
HWND f_label;
HWND output_label;
HWND ram_label;
HWND rom_label;
HWND sp_label;
/*
   End controls Declaration
*/

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    //Likely this is where the backend code will be initialized and stuff (IE, where our processor class and such will be instantiated and prepared
    emu_cpu = new Processor(); //using default instatiation for now

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY6502SIM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY6502SIM));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY6502SIM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY6502SIM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
    

    switch (message)
    {
    case WM_CREATE: {
        //Initialize my controls
            
            //setup register readouts
            a_label = CreateWindow(L"Static", L"Accumulator:", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 20, 85, 20, hWnd, (HMENU)1, NULL, NULL);
            a_reg_readout = CreateWindow(L"Static", L"00000000", WS_CHILD | WS_VISIBLE | SS_LEFT, 120, 20, 65, 20, hWnd, (HMENU) 1, NULL, NULL);
            x_label = CreateWindow(L"Static", L"X Register: ", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 50, 85, 20, hWnd, (HMENU) 1, NULL, NULL);
            x_reg_readout = CreateWindow(L"Static", L"00000000", WS_CHILD | WS_VISIBLE | SS_LEFT, 120, 50, 65, 20, hWnd, (HMENU)1, NULL, NULL);
            y_label = CreateWindow(L"Static", L"Y Register: ", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 80, 85, 20, hWnd, (HMENU)1, NULL, NULL);
            y_reg_readout = CreateWindow(L"Static", L"00000000", WS_CHILD | WS_VISIBLE | SS_LEFT, 120, 80, 65, 20, hWnd, (HMENU)1, NULL, NULL);
            pch_label = CreateWindow(L"Static", L"PC High:", WS_CHILD | WS_VISIBLE | SS_LEFT, 200, 20, 85, 20, hWnd, (HMENU)1, NULL, NULL);
            pch_readout = CreateWindow(L"Static", L"00000000", WS_CHILD | WS_VISIBLE | SS_LEFT, 320, 20, 65, 20, hWnd, (HMENU)1, NULL, NULL);
            pcl_label = CreateWindow(L"Static", L"PC Low: ", WS_CHILD | WS_VISIBLE | SS_LEFT, 200, 50, 85, 20, hWnd, (HMENU)1, NULL, NULL);
            pcl_readout = CreateWindow(L"Static", L"00000000", WS_CHILD | WS_VISIBLE | SS_LEFT, 320, 50, 65, 20, hWnd, (HMENU)1, NULL, NULL);
            sp_label = CreateWindow(L"Static", L"Stack Point: ", WS_CHILD | WS_VISIBLE | SS_LEFT, 200, 80, 85, 20, hWnd, (HMENU)1, NULL, NULL);
            sp_reg_readout = CreateWindow(L"Static", L"00000000", WS_CHILD | WS_VISIBLE | SS_LEFT, 320, 80, 65, 20, hWnd, (HMENU)1, NULL, NULL);
            f_label = CreateWindow(L"Static", L"Flags", WS_CHILD | WS_VISIBLE | SS_CENTER, 160, 120, 65, 20, hWnd, (HMENU)1, NULL, NULL);

            //flags setup
            f_n_label = CreateWindow(L"Static", L"N", WS_CHILD | WS_VISIBLE | SS_CENTER, 20 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_v_label = CreateWindow(L"Static", L"V", WS_CHILD | WS_VISIBLE | SS_CENTER, 50 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_b_label = CreateWindow(L"Static", L"B", WS_CHILD | WS_VISIBLE | SS_CENTER, 80 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_d_label = CreateWindow(L"Static", L"D", WS_CHILD | WS_VISIBLE | SS_CENTER, 110 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_i_label = CreateWindow(L"Static", L"I", WS_CHILD | WS_VISIBLE | SS_CENTER, 140 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_z_label = CreateWindow(L"Static", L"Z", WS_CHILD | WS_VISIBLE | SS_CENTER, 170 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_c_label = CreateWindow(L"Static", L"C", WS_CHILD | WS_VISIBLE | SS_CENTER, 200 + 70, 150, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_n_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 20 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_v_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 50 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_b_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 80 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_d_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 110 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_i_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 140 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_z_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 170 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);
            f_c_readout = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE | SS_CENTER, 200 + 70, 180, 20, 20, hWnd, (HMENU)1, NULL, NULL);

            //step button setup

            step_button = CreateWindow(L"BUTTON", L"STEP", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 150, 210, 80, 40, hWnd, (HMENU)ID_STEPBUTTON, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL); // not entirely sure what the longPtr does, but This should create a button that does things
        } 
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About); //call the callback
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_FILE_RESETPROCESSOR:
                //bring up dialog box for resetting the processor
                DialogBox(hInst, MAKEINTRESOURCE(IDD_RESETDIALOG), hWnd, ResetDialog);
                break;
            case ID_STEPBUTTON:
                //do button stuff here
                DialogBox(hInst, MAKEINTRESOURCE(IDD_TESTDIALOG), hWnd, TestDialog);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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

INT_PTR CALLBACK ResetDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            emu_cpu->reset(); //reset CPU
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam == IDCANCEL)) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK TestDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return(INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}