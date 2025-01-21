// windows header files
#include<windows.h>
#include"VK.h"
#include<stdio.h> // for file IO
#include<stdlib.h> // for exit
#include<winuser.h>

// Macros
#define Win_WIDTH 800
#define Win_HEIGHT 600

//global function declaration
LRESULT CALLBACK Wndproc(HWND, UINT, WPARAM, LPARAM);

const char* gpszAppName = "ARTR";

// global variable declaration
FILE* gpFile = NULL; 


HWND ghwnd = NULL;
BOOL gbActive = FALSE;
DWORD dwstyle = 0;
WINDOWPLACEMENT wprev;
BOOL gbFullscreen = FALSE;


//entry_point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declaration 
	int initialise(void);
	void uninitialise(void);
	void display(void);
	void update(void);

	// local variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[255];
	int iResult = 0;
	BOOL bDone = FALSE;
	//int height = 600;
	//int width = 800;
	int window_x_coordinate = GetSystemMetrics(SM_CXSCREEN);
	int window_y_coordinate = GetSystemMetrics(SM_CYSCREEN);
	int y;
	int x;
	x = (window_x_coordinate / 2) - Win_WIDTH / 2;
	y = (window_y_coordinate / 2) - Win_HEIGHT / 2;

	//code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("log file cannot be open"), TEXT("Eroor"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "programm started successfully...\n");

	wsprintf(szAppName, TEXT("%s"), gpszAppName);

	//WndclassEX initialisation

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = Wndproc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	//register wndclassEX

	RegisterClassEx(&wndclass);


	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
		szAppName,
		TEXT("PRS : Vulkun"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,


		x,// upperleft x-coordinate
		y,// upperleft y-coordinate
		Win_WIDTH,// width
		Win_HEIGHT,// height

		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// INITIALISATION

	iResult = initialise();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialise function failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
	}
	// show the window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	

	// gameloop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;

			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}

		}

		else
		{
			if (gbActive == TRUE)
			{
				// RENDER
				display();

				// update
				update();
			}

		}
	}

	// UNINITIALISATION
	uninitialise();

	return((int)msg.wParam);
}


//callback function
LRESULT CALLBACK Wndproc(HWND hwnd , UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void ToggleFullscreen(void);
	void resize(int, int);

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActive = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActive = FALSE;
		break;
	case WM_CREATE:
		memset(&wprev, 0, sizeof(WINDOWPLACEMENT));
		wprev.length = sizeof(WINDOWPLACEMENT);
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;


	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;

	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullscreen(void)
{
	// local variable decalration
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code
	if (gbFullscreen == FALSE)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwstyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wprev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwstyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wprev);
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);

	}

}

int initialise(void)
{
	// function declarations

	// code

	return(0);

}

void resize(int width, int heigth)
{
	// code
	if (heigth <= 0)
		heigth = 1;
}

void display(void)
{
	// code

}

void update(void)
{
	// code
}

void uninitialise(void)
{
	// function declaration
	void ToggleFullscreen(void);
	//code
	//if appliacation is exitting in fullscreen
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	// Destroywindow
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// closed log file
	if (gpFile)

	{
		fprintf(gpFile, "programm is sucessfully ended\n");
		fclose(gpFile);
		gpFile = NULL;


	}
}


