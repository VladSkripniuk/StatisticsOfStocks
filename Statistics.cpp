#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

#include "Statistics.h"
#include "resource.h"
#include "StocksStat.h"
#include "File.h"

bool beta_comp(const StocksStat &a, const StocksStat &b) {
	return a.beta > b.beta;
}

bool Sharpe_comp(const StocksStat &a, const StocksStat &b) {
	return a.Sharpe_ratio > b.Sharpe_ratio;
}

bool Treynor_comp(const StocksStat &a, const StocksStat &b) {
	return a.Treynor_ratio > b.Treynor_ratio;
}

bool JensensAlpha_comp(const StocksStat &a, const StocksStat &b) {
	return a.Jensens_Alpha > b.Jensens_Alpha;
}

bool trackingerror_comp(const StocksStat &a, const StocksStat &b) {
	return a.tracking_error > b.tracking_error;
}

bool informationratio_comp(const StocksStat &a, const StocksStat &b) {
	return a.information_ratio > b.information_ratio;
}

bool ticker_comp(const StocksStat &a, const StocksStat &b) {
	return a.ticker < b.ticker;
}

bool benchmark_comp(const StocksStat &a, const StocksStat &b) {
	return a.benchmark_index < b.benchmark_index;
}

void MakeString(StocksStat &Stat, wchar_t *text) {
	char multibyte_text[200];
	if (Stat.input_completed_successfully) {
		sprintf(multibyte_text, "%s:%s %7.4lf %8.4lf %12.4lf %14.4lf %16.4lf %18.4lf %18.4lf %16.4lf", Stat.ticker.c_str(),
			Stat.benchmark_index.c_str(), Stat.beta, Stat.alpha, Stat.Sharpe_ratio,
			Stat.Treynor_ratio, Stat.Jensens_Alpha,
			Stat.tracking_error, Stat.information_ratio, Stat.volatility_GARCH_method);
		MultiByteToWideChar(CP_ACP, NULL, multibyte_text, -1,text, 200);
	}
	else {
		sprintf(multibyte_text, "%s:%s no data", Stat.ticker.c_str(), Stat.benchmark_index.c_str());
		MultiByteToWideChar(CP_ACP, NULL, multibyte_text, -1, text, 200);
	}
}

BOOL CALLBACK StatisticsDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static std::vector<StocksStat> stocks;
	switch (uMsg) {
	case WM_INITDIALOG:
		stocks = *((std::vector<StocksStat>*)lParam);
		for (int i = 0; i < stocks.size(); ++i) {
			wchar_t text[200];
			MakeString(stocks[i], text);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_TICKER:
			std::sort(stocks.begin(), stocks.end(), ticker_comp);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < stocks.size(); ++i) {
				wchar_t text[200];
				MakeString(stocks[i], text);
				SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
			}
			return TRUE;

		case IDC_BETA:
			std::sort(stocks.begin(), stocks.end(), beta_comp);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < stocks.size(); ++i) {
				wchar_t text[200];
				MakeString(stocks[i], text);
				SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
			}
			return TRUE;

		case IDC_SHARPERATIO:
			std::sort(stocks.begin(), stocks.end(), Sharpe_comp);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < stocks.size(); ++i) {
				wchar_t text[200];
				MakeString(stocks[i], text);
				SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
			}
			return TRUE;

		case IDC_TREYNORRATIO:
			std::sort(stocks.begin(), stocks.end(), Treynor_comp);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < stocks.size(); ++i) {
				wchar_t text[200];
				MakeString(stocks[i], text);
				SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
			}
			return TRUE;

		case IDC_JENSENSALPHA:
			std::sort(stocks.begin(), stocks.end(), JensensAlpha_comp);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < stocks.size(); ++i) {
				wchar_t text[200];
				MakeString(stocks[i], text);
				SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
			}
			return TRUE;

		case IDC_INFORMATIONRATIO:
			std::sort(stocks.begin(), stocks.end(), informationratio_comp);
			SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
			for (int i = 0; i < stocks.size(); ++i) {
				wchar_t text[200];
				MakeString(stocks[i], text);
				SendMessage(GetDlgItem(hWnd, IDC_LIST1), LB_ADDSTRING, 0, (LPARAM)text);
			}
			return TRUE;


		case IDFILE:
			char name_of_file[80];
			if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FILE), hWnd, FileDlgProc, (LPARAM)name_of_file) == NULL) {
				{
					std::ofstream out_file(name_of_file);
					for (int i = 0; i < stocks.size(); ++i) {
						out_file << "##########\n";
						out_file << stocks[i].ticker << ":" << stocks[i].benchmark_index << std::endl;
						if (!stocks[i].input_completed_successfully) {
							out_file << "no data\n";
						}
						else {
							out_file << "beta " << stocks[i].beta << std::endl;
							out_file << "alpha " << stocks[i].alpha << std::endl;
							out_file << "Sharpe ratio " << stocks[i].Sharpe_ratio << std::endl;
							out_file << "Treynor ratio " << stocks[i].Treynor_ratio << std::endl;
							out_file << "Jensen's Alpha " << stocks[i].Jensens_Alpha << std::endl;
							out_file << "tracking error " << stocks[i].tracking_error << std::endl;
							out_file << "information ratio " << stocks[i].information_ratio << std::endl;
							out_file << "volatility GARCH model " << stocks[i].volatility_GARCH_method << std::endl;
						}
						out_file << "##########\n\n";
					}
					out_file.close();
				}
			}
			return TRUE;

		case IDCLOSE:
			EndDialog(hWnd, NULL);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, NULL);
		return TRUE;
		break;
	}
	return FALSE;
}