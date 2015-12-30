#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>

#include "StocksStat.h"

double mathematical_expectation(const std::vector<double> &data) {
	double sum = 0;
	for (int i = 0; i < data.size(); ++i) {
		sum += data[i];
	}
	return sum / data.size();
}

double dispersion(const std::vector<double> &data) {
	double average = mathematical_expectation(data);
	double sum = 0;
	for (int i = 0; i < data.size(); ++i) {
		sum += (data[i] - average) * (data[i] - average);
	}
	return sum / data.size();
}

double RMS_deviation(const std::vector<double> &data) {
	return sqrt(dispersion(data));
}

StocksStat::StocksStat() { }

void StocksStat::read_rates() {
	const int BEGINING_EXPECTED = 0, IDENTIFICATION_OF_BLOCK = 1, PASSING_BLOCK = 2, READING_BLOCK = 3;
	int state = BEGINING_EXPECTED;
	std::ifstream clipboard("clipboard_for_downloader.txt");
	std::string tmp_string;
	input_completed_successfully = false;
	bool successful_input_stocks_rates = false;
	bool successful_input_benchmark_rates = false;
	bool flag = true;
	while (flag) {
		switch (state) {
		case BEGINING_EXPECTED:
			while (getline(clipboard, tmp_string)) {
				if (tmp_string == "##########") {
					state = IDENTIFICATION_OF_BLOCK;
					goto end;
				}
			}
			flag = false;
			break;

		case IDENTIFICATION_OF_BLOCK:{
										 std::string name;
										 clipboard >> name;
										 if (name == ticker) {
											 getline(clipboard, tmp_string);
											 state = READING_BLOCK;
											 successful_input_stocks_rates = true;
											 goto end;
										 }
										 else {
											 state = PASSING_BLOCK;
											 goto end;
										 }
		}
			break;

		case PASSING_BLOCK:
			while (getline(clipboard, tmp_string)) {
				if (tmp_string == "##########") {
					state = BEGINING_EXPECTED;
					goto end;
				}
			}
			break;

		case READING_BLOCK:
			while (clipboard >> tmp_string) {
				if (tmp_string == "##########") {
					state = BEGINING_EXPECTED;
					goto end;
				}
				double value;
				clipboard >> value;
				stocks_rates.push_back(value);
			}
			break;
		}
	end:;
	}
	clipboard.close();
	clipboard.open("clipboard_for_downloader.txt");
	flag = true;
	while (flag) {
		switch (state) {
		case BEGINING_EXPECTED:
			while (getline(clipboard, tmp_string)) {
				if (tmp_string == "##########") {
					state = IDENTIFICATION_OF_BLOCK;
					goto end1;
				}
			}
			flag = false;
			break;

		case IDENTIFICATION_OF_BLOCK:{
										 std::string name;
										 clipboard >> name;
										 if (name == benchmark_index) {
											 getline(clipboard, tmp_string);
											 state = READING_BLOCK;
											 successful_input_benchmark_rates = true;
											 goto end1;
										 }
										 else {
											 state = PASSING_BLOCK;
											 goto end1;
										 }
		}
			break;

		case PASSING_BLOCK:
			while (getline(clipboard, tmp_string)) {
				if (tmp_string == "##########") {
					state = BEGINING_EXPECTED;
					goto end1;
				}
			}
			break;

		case READING_BLOCK:
			while (clipboard >> tmp_string) {
				if (tmp_string == "нет") {
					successful_input_benchmark_rates = false;
					state = PASSING_BLOCK;
					goto end1;
				}
				if (tmp_string == "##########") {
					state = BEGINING_EXPECTED;
					goto end1;
				}
				double value;
				clipboard >> value;
				benchmark_rates.push_back(value);
			}
			break;
		}
	end1:;
	}
	clipboard.close();
	input_completed_successfully = (successful_input_benchmark_rates && successful_input_stocks_rates);
}

void StocksStat::calculate_statistics() {
	for (int i = 0; i < stocks_rates.size() - 1; ++i) {
		stocks_yields.push_back((stocks_rates[i + 1] - stocks_rates[i]) / stocks_rates[i]);
	}
	for (int i = 0; i < benchmark_rates.size() - 1; ++i) {
		benchmark_yields.push_back((benchmark_rates[i + 1] - benchmark_rates[i]) / benchmark_rates[i]);
	}
	int N = stocks_yields.size();
	average_stocks_yield = (stocks_rates[N - 1] - stocks_rates[0]) / stocks_rates[0]; //(pow((1 + mathematical_expectation(stocks_yields)), 365) - 1) * 100;
	average_benchmark_yield = (pow((1 + mathematical_expectation(benchmark_yields)), 365) - 1) * 100;
	double stocks_sum = 0, benchmark_sum = 0, sum_of_benchmark_squares = 0, sum_of_mul = 0;
	for (int i = 0; i < N; ++i) {
		stocks_sum += stocks_yields[i];
		benchmark_sum += benchmark_yields[i];
		sum_of_mul += stocks_yields[i] * benchmark_yields[i];
		sum_of_benchmark_squares += benchmark_yields[i] * benchmark_yields[i];
	}
	beta = (N * sum_of_mul - stocks_sum * benchmark_sum) / (N * sum_of_benchmark_squares - benchmark_sum * benchmark_sum);
	alpha = (stocks_sum - beta * benchmark_sum) / N;
	std::vector<double> credit_spread(N);
	for (int i = 0; i < N; ++i) {
		credit_spread[i] = stocks_yields[i] - RISK_FREE_RATE;
	}
	Sharpe_ratio = mathematical_expectation(credit_spread) / RMS_deviation(stocks_yields);
	Treynor_ratio = mathematical_expectation(credit_spread) / beta;
	std::vector<double> difference_between_stocks_and_benchmark_returns(N);
	for (int i = 0; i < N; ++i) {
		difference_between_stocks_and_benchmark_returns[i] = stocks_yields[i] - benchmark_yields[i];
	}
	tracking_error = RMS_deviation(difference_between_stocks_and_benchmark_returns);
	std::vector<double> benchmark_credit_spread(N);
	for (int i = 0; i < N; ++i) {
		benchmark_credit_spread[i] = benchmark_yields[i] - RISK_FREE_RATE;
	}
	expected_yield = RISK_FREE_RATE + beta * mathematical_expectation(benchmark_credit_spread);
	Jensens_Alpha = mathematical_expectation(stocks_yields) - expected_yield;
	information_ratio = (mathematical_expectation(stocks_yields) - mathematical_expectation(benchmark_yields)) / tracking_error;
	double sum_of_squares_stocks = 0;
	for (int i = 0; i < stocks_yields.size(); ++i) {
		sum_of_squares_stocks += stocks_yields[i] * stocks_yields[i];
	}
	historical_volatility = sqrt(sum_of_squares_stocks / N);
	historical_implied_volatility.push_back(0);
	for (int i = 0; i < N; ++i) {
		double implied_volatility = GARCH_gamma * historical_volatility + GARCH_alpha * stocks_yields.back() * stocks_yields.back() + GARCH_beta * historical_implied_volatility.back();
		/*for (int j = 0; j <= i; ++j) {
			implied_volatility += GARCH_alpha * stocks_yields[j] * stocks_yields[j] + GARCH_beta * historical_implied_volatility[j];
		}*/
		historical_implied_volatility.push_back(implied_volatility);
	}
	volatility_GARCH_method = sqrt(historical_implied_volatility.back());
}