#include <Windows.h>

#include "File.h"
#include "resource.h"

BOOL CALLBACK FileDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static char *user_file;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hWnd, IDC_EDIT1));
		user_file = (char*)lParam;
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			wchar_t text[80];
			SendMessage(GetDlgItem(hWnd, IDC_EDIT1), WM_GETTEXT, 80, (LPARAM)text);
			WideCharToMultiByte(CP_ACP, NULL, text, -1, (LPSTR)user_file, 80, 0, 0);
			EndDialog(hWnd, NULL);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWnd, 1);
			return TRUE;
		}
		break;
	}
	return FALSE;
}