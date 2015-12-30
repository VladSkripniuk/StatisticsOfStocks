#ifndef __STOCKSSTAT__H__
#define __STOCKSSTAT_H__

const double RISK_FREE_RATE = 0.00012; // (1 + RISK_FREE_RATE) ^ 365 = 1.045
const double GARCH_alpha = 0.33333333, GARCH_beta = 0.33333333, GARCH_gamma = 0.33333333;


class StocksStat {
public:
	StocksStat();
	void read_rates();
	void calculate_statistics();
	double alpha;
	double beta;
	double Sharpe_ratio;
	double Treynor_ratio;
	double Jensens_Alpha;
	double tracking_error;
	double information_ratio;
	double expected_yield;
	double average_stocks_yield;
	double average_benchmark_yield;
	double historical_volatility;
	double volatility_GARCH_method;
	std::string ticker;
	std::string benchmark_index;
	std::vector<double> stocks_rates;
	std::vector<double> benchmark_rates;
	std::vector<double> stocks_yields;
	std::vector<double> benchmark_yields;
	std::vector<double> historical_implied_volatility;
	bool input_completed_successfully;
};

double mathematical_expectation(const std::vector<double>&);
double dispersion(const std::vector<double>&);
double RMS_deviation(const std::vector<double>&);

#endif