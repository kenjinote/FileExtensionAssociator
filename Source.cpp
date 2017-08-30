#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib,"shlwapi")

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

TCHAR szClassName[] = TEXT("Window");

BOOL RegRelSet(LPCTSTR lpszApplicationName, LPCTSTR lpszApplicationPath, LPCTSTR lpszApplicationIconPath, DWORD nIconIndex)
{
	BOOL bReturn = FALSE;
	HKEY hTopKey;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, lpszApplicationName, 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTopKey, &dwDisposition) == ERROR_SUCCESS)
	{
		HKEY hShellKey;
		if (RegCreateKeyEx(hTopKey, TEXT("shell"), 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hShellKey, &dwDisposition) == ERROR_SUCCESS)
		{
			HKEY hOpenKey;
			if (RegCreateKeyEx(hShellKey, TEXT("open"), 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hOpenKey, &dwDisposition) == ERROR_SUCCESS)
			{
				HKEY hCommandKey;
				if (RegCreateKeyEx(hOpenKey, TEXT("command"), 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hCommandKey, &dwDisposition) == ERROR_SUCCESS)
				{
					TCHAR szRegString[MAX_PATH];
					wsprintf(szRegString, TEXT("\"%s\" \"%%1\""), lpszApplicationPath);
					DWORD dwSize = sizeof(TCHAR) * (lstrlen(szRegString) + 1);
					if (RegSetValueEx(hCommandKey, NULL, 0, REG_SZ, (const unsigned char *)szRegString, dwSize) == ERROR_SUCCESS)
					{
						if (lpszApplicationIconPath)
						{
							HKEY hIconKey;
							if (RegCreateKeyEx(hTopKey, TEXT("DefaultIcon"), 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hIconKey, &dwDisposition) == ERROR_SUCCESS)
							{
								wsprintf(szRegString, TEXT("%s,%d"), lpszApplicationIconPath, nIconIndex);
								dwSize = sizeof(TCHAR) * (lstrlen(szRegString) + 1);
								if (RegSetValueEx(hIconKey, NULL, 0, REG_SZ, (const unsigned char *)szRegString, dwSize) == ERROR_SUCCESS)
								{
									bReturn = TRUE;
								}
								RegCloseKey(hIconKey);
							}
						}
					}
					RegCloseKey(hCommandKey);
				}
				RegCloseKey(hOpenKey);
			}
			RegCloseKey(hShellKey);
		}
		RegCloseKey(hTopKey);
	}
	return bReturn;
}

BOOL RegExtSet(LPCTSTR lpszApplicationName, LPCTSTR lpszApplicationExt)
{
	BOOL bReturn = FALSE;
	HKEY hTopKey;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, lpszApplicationExt, 0, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTopKey, &dwDisposition) == ERROR_SUCCESS)
	{
		DWORD dwType, dwSize = MAX_PATH;
		BYTE szRegString[MAX_PATH];
		if (RegQueryValueEx(hTopKey, lpszApplicationName, 0, &dwType, szRegString, &dwSize) == ERROR_SUCCESS)
		{
			bReturn = TRUE;
		}
		else
		{
			dwSize = sizeof(TCHAR) * (lstrlen(lpszApplicationName) + 1);
			if (RegSetValueEx(hTopKey, NULL, 0, REG_SZ, (CONST BYTE *)lpszApplicationName, dwSize) == ERROR_SUCCESS)
			{
				bReturn = TRUE;
			}
			RegCloseKey(hTopKey); return(TRUE);
		}
		RegCloseKey(hTopKey);
	}
	return bReturn;
}

DWORD DeleteRegKey(HKEY hStartKey, LPTSTR pKeyName)
{
	DWORD   dwRtn, dwSubKeyLength;
	LPTSTR  pSubKey = NULL;
	TCHAR   szSubKey[MAX_PATH];
	HKEY    hKey;
	if (pKeyName &&  lstrlen(pKeyName))
	{
		if ((dwRtn = RegOpenKeyEx(hStartKey, pKeyName, 0, KEY_ENUMERATE_SUB_KEYS | DELETE, &hKey)) == ERROR_SUCCESS)
		{
			while (dwRtn == ERROR_SUCCESS)
			{
				dwSubKeyLength = _MAX_PATH;
				dwRtn = RegEnumKeyEx(hKey, 0, szSubKey, &dwSubKeyLength, NULL, NULL, NULL, NULL);
				if (dwRtn == ERROR_NO_MORE_ITEMS)
				{
					dwRtn = RegDeleteKey(hStartKey, pKeyName);
					break;
				}
				else if (dwRtn == ERROR_SUCCESS)
				{
					dwRtn = DeleteRegKey(hKey, szSubKey);
				}
			}
			RegCloseKey(hKey);
		}
	}
	else
	{
		dwRtn = ERROR_BADKEY;
	}
	return dwRtn;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton1;
	static HWND hButton2;
	static HWND hEdit1;
	static HWND hEdit2;
	static HWND hEdit3;
	static HWND hEdit4;
	switch (msg)
	{
	case WM_CREATE:
		CreateWindow(TEXT("STATIC"), TEXT("アプリケーション名:"), WS_VISIBLE | WS_CHILD, 10, 10, 256, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("SampleAppName"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("STATIC"), TEXT("拡張子:"), WS_VISIBLE | WS_CHILD, 10, 50, 256, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(".sam"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("STATIC"), TEXT("アプリケーションパス:"), WS_VISIBLE | WS_CHILD, 10, 90, 256, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("C:\\Windows\\System32\\notepad.exe"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		CreateWindow(TEXT("STATIC"), TEXT("アプリケーションアイコンパス:"), WS_VISIBLE | WS_CHILD, 10, 130, 256, 32, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit4 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("C:\\Windows\\System32\\notepad.exe"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("登録"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("解除"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDCANCEL, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 276, 10, LOWORD(lParam) - 286, 32, TRUE);
		MoveWindow(hEdit2, 276, 50, LOWORD(lParam) - 286, 32, TRUE);
		MoveWindow(hEdit3, 276, 90, LOWORD(lParam) - 286, 32, TRUE);
		MoveWindow(hEdit4, 276, 130, LOWORD(lParam) - 286, 32, TRUE);
		MoveWindow(hButton1, 10, 170, 256, 32, TRUE);
		MoveWindow(hButton2, 276, 170, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			TCHAR szApplicationName[MAX_PATH];
			GetWindowText(hEdit1, szApplicationName, _countof(szApplicationName));
			if (szApplicationName[0] == 0)
			{
				MessageBox(hWnd, TEXT("アプリケーション名が入力されていません。"), 0, 0);
				SetFocus(hEdit1);
				return 0;
			}
			TCHAR szApplicationExt[MAX_PATH];
			GetWindowText(hEdit2, szApplicationExt, _countof(szApplicationExt));
			if (szApplicationExt[0] != TEXT('.') || lstrlen(szApplicationExt) <= 1)
			{
				MessageBox(hWnd, TEXT("拡張子が正しく入力されていません。"), 0, 0);
				SetFocus(hEdit2);
				return 0;
			}
			TCHAR szApplicationPath[MAX_PATH];
			GetWindowText(hEdit3, szApplicationPath, _countof(szApplicationPath));
			if (PathFileExists(szApplicationPath) == FALSE)
			{
				MessageBox(hWnd, TEXT("アプリケーションのパスが正しくありません。"), 0, 0);
				SendMessage(hEdit3, EM_SETSEL, 0, -1);
				SetFocus(hEdit3);
				return 0;
			}
			TCHAR szApplicationIconPath[MAX_PATH];
			GetWindowText(hEdit4, szApplicationIconPath, _countof(szApplicationIconPath));
			if (PathFileExists(szApplicationIconPath) == FALSE)
			{
				MessageBox(hWnd, TEXT("アプリケーションアイコンのパスが正しくありません。"), 0, 0);
				SendMessage(hEdit4, EM_SETSEL, 0, -1);
				SetFocus(hEdit4);
				return 0;
			}
			if (LOWORD(wParam) == IDOK)
			{
				TCHAR szShortPath[MAX_PATH];
				GetShortPathName(szApplicationIconPath, szShortPath, MAX_PATH);
				RegRelSet(szApplicationName, szApplicationPath, szShortPath, 0);
				RegExtSet(szApplicationName, szApplicationExt);
			}
			else if (LOWORD(wParam) == IDCANCEL)
			{
				DeleteRegKey(HKEY_CLASSES_ROOT, szApplicationName);
				DeleteRegKey(HKEY_CLASSES_ROOT, szApplicationExt);
			}
			HKEY hKey;
			RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSavePidlMRU"), 0, KEY_ALL_ACCESS, &hKey);
			DeleteRegKey(hKey, &szApplicationExt[1]);
			RegCloseKey(hKey);
			RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts"), 0, KEY_ALL_ACCESS, &hKey);
			DeleteRegKey(hKey, szApplicationExt);
			RegCloseKey(hKey);
			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("拡張子の関連付け/関連付け解除を行う"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
