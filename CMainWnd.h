#ifndef __MAINWND__H__
#define __MAINWND__H__

#include <vector>

#define ID_ADD (WM_APP)
#define ID_EDIT (WM_APP + 1)
#define ID_ADDED (WM_APP + 2)
#define ID_DEL (WM_APP + 3)
#define ID_DELALL (WM_APP + 4)
#define ID_CALC (WM_APP + 5)
#define ID_ADDFROMFILE (WM_APP + 6)
#define ID_SAVETOFILE (WM_APP + 7)
#define ID_FROMFILE (WM_APP + 8)

class CMainWnd {
public:
	CMainWnd(int nArgCmdShow);
	void Show();
	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticAddButtonProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK AddButtonProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticEditProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK EditProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticHintProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK HintProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticAddedListProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK AddedListProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticDelButtonProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK DelButtonProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticDelAllButtonProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK DelAllButtonProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticCalcButtonProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK CalcButtonProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticAddFromFileProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK AddFromFileProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK StaticSaveToFileProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK SaveToFileProc(HWND, UINT, WPARAM, LPARAM);
	void ShowHint(TCHAR *prefix);
private:
	WNDCLASSEX wc;
	std::vector<HWND> vectorOfControls;
	int nFocusedControl = 0;
	HINSTANCE hInstance;
	int nCmdShow;
	HWND hMainWnd, hAddButton, hEditWnd, hHintWnd,
		hAddedList, hDelButton, hDelAllButton,
		hCalcButton, hAddFromFile, hSaveToFile, hFromFile;
	WNDPROC lpfnDefaultListProc;
	WNDPROC lpfnDefaultButtonProc;
	WNDPROC lpfnDefaultEditProc;
};

#endif