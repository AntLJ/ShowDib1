// ShowDib1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ShowDib1.h"
#include "DibFile.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SHOWDIB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHOWDIB1));

    MSG msg;

    // メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHOWDIB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SHOWDIB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BITMAPFILEHEADER * pbmfh;
	static BITMAPINFO * pbmi;
	static BYTE * pBits;
	static int cxClient, cyClient, cxDib, cyDib;
	static TCHAR szFileName[MAX_PATH], szTitleName
		[MAX_PATH];
	BOOL bSuccess;
	HDC hdc;
	PAINTSTRUCT ps;

    switch (message)
    {
	case WM_CREATE:
		DibFileInitialize(hwnd);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	case WM_INITMENUPOPUP:
		EnableMenuItem((HMENU)wParam, IDM_FILE_SAVE, pbmfh ? MF_ENABLED : MF_GRAYED);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_OPEN:
			// Show the File Open dialog box
			if (!DibFileOpenDlg(hwnd, szFileName, szTitleName))
				return 0;

			// If there's an existing DIB, free the memory
			if (pbmfh)
			{
				free(pbmfh);
				pbmfh = NULL;
			}
			// Load the entire DIB into memory
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			pbmfh = DibLoadImage(szFileName);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			// Invalidate the client area for later update
			InvalidateRect(hwnd, NULL, TRUE);
			if (pbmfh == NULL)
			{
				MessageBox(hwnd, TEXT("Cannot load DIB file"), szWindowClass, 0);
				return 0;
			}
			// Get pointers to the info structure & the bits
			pbmi = (BITMAPINFO *)(pbmfh + 1);
			pBits = (BYTE *)pbmfh + pbmfh->bfOffBits;
			// Get the DIB width and height
			if (pbmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
			{
				cxDib = ((BITMAPCOREHEADER *)pbmi)->bcWidth;
				cyDib = ((BITMAPCOREHEADER *)pbmi)->bcHeight;
			}
			else
			{
				cxDib = pbmi->bmiHeader.biWidth;
				cyDib = abs(pbmi->bmiHeader.biHeight);
			}
			return 0;
		case IDM_FILE_SAVE:
			// Show the File Save dialog box
			if (!DibFileSaveDlg(hwnd, szFileName, szTitleName))
				return 0;

			// Save the DIB to memory
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ShowCursor(TRUE);
			bSuccess = DibSaveImage(szFileName, pbmfh);
			ShowCursor(FALSE);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (!bSuccess)
				MessageBox(hwnd, TEXT("Cannot save DIB file"), szWindowClass, 0);
			return 0;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (pbmfh)
			SetDIBitsToDevice(hdc,
				0, // xDst
				0, // yDst
				cxDib, // cxSrc
				cyDib, // cySrc
				0, // xSrc
				0, // ySrc
				0, // first scan line
				cyDib / 2, // number of scan lines
				pBits,
				pbmi,
				DIB_RGB_COLORS);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		if (pbmfh)
			free(pbmfh);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

// バージョン情報ボックスのメッセージ ハンドラーです。
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
