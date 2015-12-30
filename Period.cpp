#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <TCHAR.H>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <locale>

#include "Period.h"
#include "resource.h"

bool CheckDate(wchar_t *text) {
	if (wcslen(text) != 21) {
		return false;
	}
	if ((text[2] != '.') || (text[5] != '.') || (text[10] != '-') || (text[13] != '.') || (text[16] != '.')) {
		return false;
	}
	if ((!isdigit(text[0])) || (!isdigit(text[1]))) {
		return false;
	}
	if ((!isdigit(text[3])) || (!isdigit(text[4]))) {
		return false;
	}
	if ((!isdigit(text[6])) || (!isdigit(text[7]))) {
		return false;
	}
	if ((!isdigit(text[8])) || (!isdigit(text[9]))) {
		return false;
	}
	if ((!isdigit(text[11])) || (!isdigit(text[12]))) {
		return false;
	}
	if ((!isdigit(text[14])) || (!isdigit(text[15]))) {
		return false;
	}
	if ((!isdigit(text[17])) || (!isdigit(text[18]))) {
		return false;
	}
	if ((!isdigit(text[19])) || (!isdigit(text[20]))) {
		return false;
	}
	return true;
}

BOOL CALLBACK PeriodDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			wchar_t text[30];
			SendMessage(GetDlgItem(hWnd, IDC_EDIT1), WM_GETTEXT, 30, (LPARAM)text);
			if (!CheckDate(text)) {
				MessageBox(hWnd, _T("invalid input"), _T("Error!"), MB_ICONWARNING | MB_OK);
			}
			else {
				text[10] = ' ';
				std::wofstream clipboard;
				clipboard.open("clipboard_for_downloader.txt", std::ios::app);
				clipboard.imbue(std::locale("rus_rus.1251"));
				clipboard << std::wstring(text);
				clipboard.close();
				EndDialog(hWnd, NULL);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, 1);
			return TRUE;
		}
		break;
	}
	return FALSE;
}