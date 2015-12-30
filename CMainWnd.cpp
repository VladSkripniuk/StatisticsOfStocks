#include <Python.h>
#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <TCHAR.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#include "CMainWnd.h"
#include "resource.h"
#include "Period.h"
#include "StocksStat.h"
#include "Statistics.h"
#include "File.h"

typedef std::basic_string<TCHAR> tstring;

CMainWnd::CMainWnd(int nArgCmdShow) {
	hInstance = GetModuleHandle(NULL);
	nCmdShow = nArgCmdShow;

	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpfnWndProc = StaticWndProc;
	wc.hInstance = hInstance;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = _T("Stocks^Statistics Main window");

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, _T("Cannot register class"), _T("Error"), MB_OK);
		return;
 	}

	hMainWnd = CreateWindow(_T("Stocks^Statistics Main window"), _T("Stocks^Statistics"),
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, 800, 450, (HWND)NULL,
		(HMENU)NULL, (HINSTANCE)hInstance, NULL);

	if (!hMainWnd) {
		MessageBox(NULL, _T("Cannot create main window"), _T("Error"), MB_OK);
		return;
	}

	SetProp(hMainWnd, _T("Object"), this);

	hAddButton = CreateWindow(_T("BUTTON"), _T("Add"),
		WS_CHILD | WS_VISIBLE,
		580, 200, 80, 20,
		hMainWnd, (HMENU)ID_ADD, hInstance, NULL);

	SetProp(hAddButton, _T("Object"), this);

	lpfnDefaultButtonProc = (WNDPROC)SetWindowLong(hAddButton, GWL_WNDPROC, (LONG)StaticAddButtonProc);

	hEditWnd = CreateWindow(_T("EDIT"), _T("enter ticker or name of company:index"),
		WS_CHILD | WS_VISIBLE,
		160, 200, 400, 20,
		hMainWnd, (HMENU)ID_EDIT, hInstance, 0);

	SetProp(hEditWnd, _T("Object"), this);

	lpfnDefaultEditProc = (WNDPROC)SetWindowLong(hEditWnd, GWL_WNDPROC, (LONG)StaticEditProc);

	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 208));;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = StaticHintProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = _T("HintWindow");

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, _T("Cannot register HintWindow class"), _T("Error"), MB_OK);
		return;
	}

	hAddedList = CreateWindow(_T("LISTBOX"), _T("list of added tickers"),
		WS_CHILD | WS_VISIBLE, 160, 20, 400, 150, hMainWnd,
		(HMENU)ID_ADDED, hInstance, NULL);
	
	SetProp(hAddedList, _T("Object"), this);

	lpfnDefaultListProc = (WNDPROC)SetWindowLong(hAddedList, GWL_WNDPROC, (LONG)StaticAddedListProc);

	hDelButton = CreateWindow(_T("BUTTON"), _T("remove"),
		WS_CHILD | WS_VISIBLE, 40, 100, 110, 20, hMainWnd,
		(HMENU)ID_DEL, hInstance, NULL);

	SetWindowLong(hDelButton, GWL_WNDPROC, (LONG)StaticDelButtonProc);
	SetProp(hDelButton, _T("Object"), this);

	hDelAllButton = CreateWindow(_T("BUTTON"), _T("remove all"),
		WS_CHILD | WS_VISIBLE, 40, 130, 110, 20, hMainWnd,
		(HMENU)ID_DELALL, hInstance, NULL);

	SetWindowLong(hDelAllButton, GWL_WNDPROC, (LONG)StaticDelAllButtonProc);
	SetProp(hDelAllButton, _T("Object"), this);

	hCalcButton = CreateWindow(_T("BUTTON"), _T("Calculate!"),
		WS_CHILD | WS_VISIBLE, 580, 80, 70, 40, hMainWnd,
		(HMENU)ID_CALC, hInstance, NULL);

	SetWindowLong(hCalcButton, GWL_WNDPROC, (LONG)StaticCalcButtonProc);
	SetProp(hCalcButton, _T("Object"), this);

	hAddFromFile = CreateWindow(_T("BUTTON"), _T("from file"),
		WS_CHILD | WS_VISIBLE, 40, 40, 110, 20, hMainWnd,
		(HMENU)ID_ADDFROMFILE, hInstance, NULL);

	SetWindowLong(hAddFromFile, GWL_WNDPROC, (LONG)StaticAddFromFileProc);
	SetProp(hAddFromFile, _T("Object"), this);

	hSaveToFile = CreateWindow(_T("BUTTON"), _T("save"),
		WS_CHILD | WS_VISIBLE, 40, 70, 110, 20, hMainWnd,
		(HMENU)ID_SAVETOFILE, hInstance, NULL);

	SetWindowLong(hSaveToFile, GWL_WNDPROC, (LONG)StaticSaveToFileProc);
	SetProp(hSaveToFile, _T("Object"), this);

	hFromFile = CreateWindow(_T("button"), _T("from file"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 580, 130, 90, 20,
		hMainWnd, (HMENU)ID_FROMFILE, hInstance, NULL);
	SendMessage(hFromFile, BM_SETCHECK, 0, 0);

	vectorOfControls.push_back(hEditWnd);
	vectorOfControls.push_back(hCalcButton);
	vectorOfControls.push_back(hAddButton);
	vectorOfControls.push_back(hAddedList);
	vectorOfControls.push_back(hAddFromFile);
	vectorOfControls.push_back(hSaveToFile);
	vectorOfControls.push_back(hDelButton);
	vectorOfControls.push_back(hDelAllButton);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);
}

void CMainWnd::Show() {
	hMainWnd = CreateWindow(_T("Stocks^Statistics Main window"), _T("Stocks^Statistics"),
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, 800, 450, (HWND)NULL,
		(HMENU)NULL, (HINSTANCE)hInstance, NULL);

	if (!hMainWnd) {
		MessageBox(NULL, _T("Cannot create main window"), _T("Error"), MB_OK);
		return;
	}

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);
}

LRESULT CALLBACK CMainWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC, hCompatibleDC;
	PAINTSTRUCT ps;
	HANDLE hBitMap, hOldBitMap;
	BITMAP Bitmap;
	RECT rect;

	switch (uMsg) {
	
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hBitMap = LoadImage(NULL, _T("Background.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		GetObject(hBitMap, sizeof(BITMAP), &Bitmap);
		hCompatibleDC = CreateCompatibleDC(hDC);
		hOldBitMap = SelectObject(hCompatibleDC, hBitMap);
		GetClientRect(hWnd, &rect);
		StretchBlt(hDC, 0, 0, rect.right, rect.bottom, hCompatibleDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);
		SelectObject(hCompatibleDC, hOldBitMap);
		DeleteObject(hBitMap);
		DeleteDC(hCompatibleDC);
		int nOldMode;
		nOldMode = SetBkMode(hDC, TRANSPARENT);
		TextOut(hDC, 160, 175, _T("all added tickers will be shown in the window above"), 52);
		SetBkMode(hDC, nOldMode);
		EndPaint(hWnd, &ps);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case ID_ADD:
			wchar_t ticker[20];
			SendMessage(hEditWnd, WM_GETTEXT, 20, (LPARAM)ticker);
			SendMessage(hAddedList, LB_ADDSTRING, 0, (LPARAM)ticker);
			break;

		case ID_DEL:
		{
					   int iIndex;
					   iIndex = SendMessage(hAddedList, LB_GETCURSEL, 0, 0);
					   SendMessage(hAddedList, LB_DELETESTRING, iIndex, 0);
		}
			break;

		case ID_DELALL:
			SendMessage(hAddedList, LB_RESETCONTENT, 0, 0);
			break;

		case ID_CALC: {
						  std::ofstream clipboard;
						  clipboard.open("clipboard_for_downloader.txt", std::ios::out);
						  int numberOfTickers;
						  numberOfTickers = SendMessage(hAddedList, LB_GETCOUNT, 0, 0);
						  std::set<std::string> used_tickers;
						  std::vector<StocksStat> stocks(numberOfTickers);
						  for (int index = 0; index < numberOfTickers; ++index) {
							  TCHAR wText[20];
							  char text[20];
							  SendMessage(hAddedList, LB_GETTEXT, index, (LPARAM)wText);
							  WideCharToMultiByte(CP_ACP, NULL, wText, -1, text, 20, 0, 0);
							  int pos = -1, len = strlen(text);
							  for (int i = 0; i < len; ++i) {
								  if (text[i] == ':') {
									  pos = i;
									  break;
								  }
							  }
							  text[pos] = '\0';
							  stocks[index].ticker = std::string(text);
							  stocks[index].benchmark_index = std::string(text + pos + 1);
							  if (used_tickers.find(std::string(text)) == used_tickers.end()) {
								  used_tickers.insert(std::string(text));
								  if (index == 0) {
									  clipboard << std::string(text);
								  }
								  else {
									  clipboard << ", " << std::string(text);
								  }
								  if (used_tickers.find(std::string(text + pos + 1)) == used_tickers.end()) {
									  clipboard << ", " << std::string(text + pos + 1);
									  used_tickers.insert(std::string(text + pos + 1));
								  }
							  }
						  }
						  clipboard << '\n';
						  clipboard.close();
						  if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_PERIOD), hMainWnd, PeriodDlgProc) == NULL) {
							  if (SendMessage(hFromFile, BM_GETCHECK, 0, 0) == BST_CHECKED) {
								  char name_of_file[30];
								  if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FILE), hWnd, FileDlgProc, (LPARAM)name_of_file) == NULL) {
									  std::ifstream file(name_of_file);
									  if (!file) {
										  MessageBox(NULL, _T("file was not found"), _T("Error"), MB_ICONWARNING | MB_OK);
									  }
									  else {
										  std::ofstream clipboard("clipboard_for_downloader.txt");
										  std::string text;
										  while (getline(file, text)) {
											  clipboard << text << std::endl;
										  }
										  file.close();
										  clipboard.close();
									  }
								  }
							  }
							  else {
								  Py_Initialize();
								  PyObject *obj = Py_BuildValue("s", "downloader.py");
								  FILE *file = _Py_fopen_obj(obj, "r+");
								  if (file != NULL) {
									  PyRun_SimpleFile(file, "downloader.py");
								  }
								  Py_Finalize();
							  }
							  for (std::vector<StocksStat>::iterator iterator = stocks.begin(); iterator != stocks.end(); ++iterator) {
								  iterator->read_rates();
								  if (iterator->input_completed_successfully) {
									  iterator->calculate_statistics();
								  }
							  }
							  DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_STATISTICS), hMainWnd, StatisticsDlgProc, (LPARAM)&stocks);
						  }
		}
			break;

		case ID_ADDFROMFILE: {
								 char name_of_file[80];
								 if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FILE), hWnd, FileDlgProc, (LPARAM)name_of_file) == NULL) {
										 tstring text;
										 std::wifstream file(name_of_file);
										 if (!file) {
											 MessageBox(NULL, _T("file was not found"), _T("Error"), MB_ICONWARNING | MB_OK);
										 }
										 else {
											 file.imbue(std::locale("rus_rus.1251"));
											 while (file >> text) {
												 SendMessage(hAddedList, LB_ADDSTRING, 0, (LPARAM)text.c_str());
											 }
											 file.close();
										 }
								 }
		}
			break;

		case ID_SAVETOFILE: {
								char name_of_file[80];
								if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FILE), hWnd, FileDlgProc, (LPARAM)name_of_file) == NULL) {
										tstring text;
										std::wofstream file(name_of_file);
										int numberOfTickers = SendMessage(hAddedList, LB_GETCOUNT, 0, 0);
										for (int index = 0; index < numberOfTickers; ++index) {
											TCHAR wText[20];
											SendMessage(hAddedList, LB_GETTEXT, index, (LPARAM)wText);
											file << tstring(wText) << std::endl;
										}
										file.close();
								}
		}
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			SetFocus(vectorOfControls[nFocusedControl++]);
			nFocusedControl %= vectorOfControls.size();
			break;
		}
		break;
	
	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd,
			_T("Are you sure you want to quit?\nAll unsaved changes will be lost!"),
			_T(""), MB_YESNO | MB_ICONWARNING)) {
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*) GetProp(hWnd, _T("Object")))->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CMainWnd::AddButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN:
			PostMessage(hMainWnd, WM_COMMAND, ID_ADD, NULL);
			break;

		default:
			return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
		}

	default:
		return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticAddButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*) GetProp(hWnd, _T("Object")))->AddButtonProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::StaticEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*) GetProp(hWnd, _T("Object")))->EditProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::EditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_SETFOCUS:
		TCHAR text[80];
		GetWindowText(hWnd, text, 79);
		if (_tcscmp(text, _T("enter ticker or name of company:index")) == 0) {
			SetWindowText(hWnd, _T(""));
		}
		return lpfnDefaultEditProc(hWnd, uMsg, wParam, lParam);
		break;

	case WM_KILLFOCUS:
		GetWindowText(hWnd, text, 79);
		if (_tcscmp(text, _T("")) == 0) {
			SetWindowText(hWnd, _T("enter ticker or name of company:index"));
		}
		return lpfnDefaultEditProc(hWnd, uMsg, wParam, lParam);
		break;

	case WM_KEYDOWN:
		switch (wParam) {

		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN: {
							TCHAR text[80];
							SendMessage(hEditWnd, WM_GETTEXT, 79, (LPARAM)text);
							int len = wcslen(text);
							int cnt = 0;
							for (int i = 0; i < len; ++i) {
								if (text[i] == ':') {
									++cnt;
								}
							}
							if (cnt == 1) {
								PostMessage(hMainWnd, WM_COMMAND, ID_ADD, NULL);
							}
							else {
								MessageBox(hMainWnd, _T("invalid input!"), _T("Error!"), MB_ICONERROR | MB_OK);
							}
		}
			break;

		case VK_DOWN:
		{
						std::wofstream clipboard("clipboard_for_similarities_finder.txt");
						TCHAR text[80];
						SendMessage(hEditWnd, WM_GETTEXT, 79, (LPARAM)text);
						int pos = -1, len = wcslen(text);
						for (int i = 0; i < len; ++i) {
							if (text[i] == ':') {
								pos = i;
							}
						}
						clipboard.imbue(std::locale("rus_rus.1251"));
						clipboard << tstring(text + pos + 1);
						clipboard.close();
						Py_Initialize();
						PyObject *obj = Py_BuildValue("s", "similarities_finder.py");
						FILE *file = _Py_fopen_obj(obj, "r+");
						if (file != NULL) {
							PyRun_SimpleFile(file, "similarities_finder.py");
						}
						Py_Finalize();
						if (pos == -1) {
							ShowHint(_T(""));
						}
						else {
							text[pos] = '\0';
							ShowHint(text);
						}
						break;
		}
		default:
			return lpfnDefaultEditProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	default:
		return lpfnDefaultEditProc(hWnd, uMsg, wParam, lParam);
	}
}

void CMainWnd::ShowHint(TCHAR *prefix) {
	
	hHintWnd = CreateWindow(_T("HintWindow"), _T("Hint"), WS_CHILD | WS_VISIBLE | WS_VSCROLL,
		160, 220, 400, 200, hMainWnd, NULL, hInstance, NULL);

	SetProp(hHintWnd, _T("Object"), this);
	TCHAR *pPrefix = new TCHAR[80];
	_tcscpy(pPrefix, prefix);
	SetProp(hHintWnd, _T("prefix"), (HANDLE)pPrefix);
	
	SetFocus(hHintWnd);
	UpdateWindow(hHintWnd);
}

LRESULT CALLBACK CMainWnd::StaticHintProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->HintProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::HintProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static std::vector<std::string> hints;
	static int nCursorPosition, nXShift, nYShift, nSelectedHint, nMaxLen;
	static TEXTMETRIC tm;
	HDC hDC;
	PAINTSTRUCT ps;
	static int cxChar, yStep;
	RECT rect;
	static SCROLLINFO vsi;
	switch (uMsg) {
	case WM_CREATE:
	{
					  nMaxLen = nSelectedHint = nCursorPosition = nXShift = nYShift = 0;
					  std::ifstream clipboard("clipboard_for_similarities_finder.txt");
					  std::string hint;
					  hints.resize(0);
					  while (getline(clipboard, hint)) {
						  hints.push_back(hint);
						  if (hint.length() > nMaxLen) {
							  nMaxLen = hint.length();
						  }
					  }
					  clipboard.close();
					  hDC = GetDC(hWnd);
					  GetTextMetrics(hDC, &tm);
					  cxChar = tm.tmAveCharWidth;
					  yStep = tm.tmHeight + tm.tmExternalLeading;
					  ReleaseDC(hWnd, hDC);

					  memset(&vsi, 0, sizeof(vsi));
					  vsi.cbSize = sizeof(vsi);
					  vsi.nPage = 11;
					  vsi.nMax = max(10, int(hints.size() - 1));
					  vsi.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
					  SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
					  break;
	}
		memset(&vsi, 0, sizeof(vsi));
		vsi.cbSize = sizeof(vsi);
		vsi.nPage = 11;
		vsi.nMax = max(10, int(hints.size() - 1));
		vsi.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_DOWN:
			if (nSelectedHint < hints.size() - 1) {
				if (nCursorPosition < 10) {
					++nCursorPosition;
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
				else if (nCursorPosition == 10) {
					--nYShift;
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
				++nSelectedHint;
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			}
			break;

		case VK_UP:
			if (nSelectedHint > 0) {
				if (nCursorPosition > 0) {
					--nCursorPosition;
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
				else if (nCursorPosition == 0) {
					++nYShift;
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
				--nSelectedHint;
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			}
			break;

		case VK_LEFT:
			if (nXShift < 0) {
				++nXShift;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;

		case VK_RIGHT:
			if (-nXShift < nMaxLen) {
				--nXShift;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;

		case VK_RETURN: {
							tstring wstr = tstring((TCHAR*)GetProp(hWnd, _T("prefix")));
							char str[80];
							WideCharToMultiByte(CP_ACP, NULL, wstr.c_str(), -1, (LPSTR)str, 80, 0, 0);
							if (strlen(str) == 0) {
								SetWindowTextA(hEditWnd, (hints[nSelectedHint].substr(0, hints[nSelectedHint].find(' '))).c_str());
							}
							else {
								SetWindowTextA(hEditWnd, (std::string(str) + ":" + hints[nSelectedHint].substr(0, hints[nSelectedHint].find(' '))).c_str());
							}
							SetFocus(hEditWnd);
		}
			break;

		case VK_ESCAPE:
			SetFocus(hEditWnd);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;

	case WM_VSCROLL:
		switch (LOWORD(wParam)) {
		case SB_LINEUP:
			--vsi.nPos;
			SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
			break;

		case SB_LINEDOWN:
			++vsi.nPos;
			SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
			break;
		}
		break;

	case WM_PAINT: {
					   hDC = BeginPaint(hWnd, &ps);
					   GetClientRect(hWnd, &rect);
					   HRGN hRgn = CreateRectRgnIndirect(&rect);
					   SelectClipRgn(hDC, hRgn);
					   DeleteObject(hRgn);
					   int hOldMode = SetBkMode(hDC, TRANSPARENT);
					   GetClientRect(hWnd, &rect);
					   rect.left += cxChar;
					   rect.right -= cxChar;
					   int x, y;
					   x = cxChar * (nXShift + 1);
					   y = yStep * nYShift;
					   for (int i = 0; i < hints.size(); ++i) {
						   TextOutA(hDC, x, y, hints[i].c_str(), hints[i].size());
						   y += yStep;
					   }
					   SetBkMode(hDC, hOldMode);
					   int hOldROP2 = SetROP2(hDC, R2_XORPEN);
					   rect.left -= cxChar;
					   rect.right += cxChar;
					   rect.top = nCursorPosition * yStep;
					   rect.bottom = (nCursorPosition + 1) * yStep;
					   Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
					   SetROP2(hDC, hOldROP2);
					   EndPaint(hWnd, &ps);
					   break;
	}
	case WM_KILLFOCUS:
		DestroyWindow(hWnd);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK CMainWnd::StaticAddedListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->AddedListProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::AddedListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_DELETE:
		{
						  int iIndex = SendMessage(hAddedList, LB_GETCURSEL, NULL, NULL);
						  SendMessage(hAddedList, LB_DELETESTRING, iIndex, NULL);
						  break;
		}
		default:
			return lpfnDefaultListProc(hWnd, uMsg, wParam, lParam);
		}

		break;

	default:
		return lpfnDefaultListProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticDelButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->DelButtonProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::DelButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN:
			PostMessage(hMainWnd, WM_COMMAND, ID_DEL, NULL);
			break;

		default:
			return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
		}

	default:
		return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticDelAllButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->DelAllButtonProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::DelAllButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN:
			PostMessage(hMainWnd, WM_COMMAND, ID_DELALL, NULL);
			break;

		default:
			return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
		}

	default:
		return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticCalcButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->CalcButtonProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::CalcButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN:
			PostMessage(hMainWnd, WM_COMMAND, ID_CALC, NULL);
			break;

		default:
			return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
		}

	default:
		return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticAddFromFileProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->AddFromFileProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::AddFromFileProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN:
			PostMessage(hMainWnd, WM_COMMAND, ID_ADDFROMFILE, NULL);
			break;

		default:
			return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
		}

	default:
		return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CMainWnd::StaticSaveToFileProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return ((CMainWnd*)GetProp(hWnd, _T("Object")))->SaveToFileProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CMainWnd::SaveToFileProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_TAB:
			PostMessage(hMainWnd, WM_KEYDOWN, VK_TAB, NULL);
			break;

		case VK_RETURN:
			PostMessage(hMainWnd, WM_COMMAND, ID_SAVETOFILE, NULL);
			break;

		default:
			return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
		}

	default:
		return CallWindowProc(lpfnDefaultButtonProc, hWnd, uMsg, wParam, lParam);
	}
}