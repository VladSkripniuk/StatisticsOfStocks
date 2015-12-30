def from_finam(ticker):
	finam_ticker_list = open("finam_ticker_list.txt", "r")
	for line in finam_ticker_list:
		if line.split(",")[1][1:-1] == ticker:
			return True
	finam_ticker_list.close()
	return False

def from_index(ticker):
	index = open("index_list.txt", "r")
	for line in index:
		if ticker == line.rstrip():
			return True
	index.close()
	return False

from urllib import request
from datetime import datetime

def download_from_finam(ticker, period):
	start_date = period.split(" ")[0]
	end_date = period.split(" ")[1]
	finam_ticker_list = open("finam_ticker_list.txt", "r")
	for line in finam_ticker_list:
		if ticker == line.split(",")[1][1:-1]:
			symbol_code = line.split(",")[0]
	symbol = ticker
	finam_ticker_list.close()
	url = "http://195.128.78.52/{0}.csv?market=1&em={1}&code={0}&df={2}&mf={3}&yf={4}&from={5}&dt={6}&mt={7}&yt={8}&to={9}&p=8&f={0}&e=.csv&cn={0}&dtf=1&tmf=3&MSOR=1&mstime=on&mstimever=1&sep=3&sep2=1&datf=5&at=1".format(ticker, symbol_code, start_date.split(".")[0],\
		int(start_date.split(".")[1]) - 1, start_date.split(".")[2], start_date, end_date.split(".")[0],\
		int(end_date.split(".")[1]) - 1, end_date.split(".")[2], end_date)
	out_file = open("url.txt", "w")
	out_file.write(url)
	out_file.close()
	f = request.urlopen(url)
	data = f.read()
	data = str(data)
	data = data.split("\\r\\n")
	dates_and_rates = [(line.split(";")[0], line.split(";")[5]) for line in data[1:-1]]
	dates_and_rates = [(".".join((date[0][6:8], date[0][4:6], date[0][0:4])), float(date[1])) for date in dates_and_rates]
	f.close()
	return dates_and_rates

import ystockquote

def download_from_yahoo(ticker, period):
	beg_date = period.split(" ")[0]
	end_date = period.split(" ")[1]
	beg_date = beg_date.split(".")
	beg_date.reverse()
	end_date = end_date.split(".")
	end_date.reverse()
	data = ystockquote.get_historical_prices(ticker, "-".join(beg_date), "-".join(end_date))
	data = [(day, data[day]["Close"])for day in sorted(data.keys())]
	data = [(".".join((date[0][8:10], date[0][5:7], date[0][0:4])), float(date[1])) for date in data]
	return data

from urllib import error

class date:
	day = month = year = 0
	def date_from_string(self, string):
		self.day = int(string[0:2])
		self.month = int(string[3:5])
		self.year = int(string[6:10])
	def string_from_date(self):
		result = ""
		if self.day < 10:
			result += "0"
		result += str(self.day)
		result += '.'
		if self.month < 10:
			result += "0"
		result += str(self.month)
		result += '.'
		result += str(self.year)
		return result

	def inc(self):
		if self.month in set([1, 3, 5, 7, 8, 10]):
			if self.day == 31:
				self.day = 1
				self.month += 1
			else:
				self.day += 1
			return 0
		if self.month in set([4, 6, 9, 11]):
			if self.day == 30:
				self.day = 1
				self.month += 1
			else:
				self.day += 1
			return 0
		if self.month == 2:
			if self.year % 400 == 0 or (self.year % 4 == 0 and self.year % 100 != 0):
				if self.day == 29:
					self.month += 1
					self.day = 1
				else:
					self.day += 1
			else:
				if self.day == 28:
					self.month += 1
					self.day = 1
				else:
					self.day += 1
			return 0
		if self.month == 12:
			if self.day == 31:
				self.month = 1
				self.year += 1
				self.day = 1
			else:
				self.day += 1
			return 0


	def dec(self):
		if self.day == 1:
			if self.month == 1:
				self.month = 12
				self.day = 31
				self.year -= 1
			else:
				self.month -= 1
				if self.month in set([1, 3, 5, 7, 8, 10]):
					self.day = 31
					return 0
				if self.month in set([4, 6, 9, 11]):
					self.day = 30
					return 0
				if self.month == 2:
					if self.year % 400 == 0 or (self.year % 4 == 0 and self.year % 100 != 0):
						self.day = 29
					else:
						self.day = 28
					return 0
		else:
			self.day -= 1

def comp_dates(date1, date2):
	if date1.day == date2.day and date1.month == date2.month and date1.year == date2.year:
		return True
	else:
		return False

def more_date(date1, date2):
	if (date1.year > date2.year):
		return True
	if (date1.year < date2.year):
		return False;
	if (date1.month > date2.month):
		return True
	if (date1.month < date2.month):
		return False;
	if (date1.day > date2.day):
		return True
	else:
		return False;

def linear_interpolation(begin, end):
	result = []
	date_iterator = date()
	date_iterator.date_from_string(begin[0])
	len_of_period = 0
	last_date = date()
	last_date.date_from_string(end[0])
	while not comp_dates(date_iterator, last_date):
		date_iterator.inc()
		len_of_period += 1
	date_iterator.date_from_string(begin[0])
	for i in range(len_of_period):
		result.append((date_iterator.string_from_date(), ((len_of_period - i) * begin[1] + i * end[1]) / len_of_period))
		date_iterator.inc()
	return result

def fill_in_gaps(rates, period):
	result = []
	beg_date = date()
	beg_date.date_from_string(period[:10])
	end_date = date()
	end_date.date_from_string(period[11:21])
	date_iterator = date()
	date_iterator.date_from_string(rates[0][0])
	while not comp_dates(date_iterator, beg_date):
		date_iterator.dec()
		result.append((date_iterator.string_from_date(), rates[0][1]))
	result.reverse()
	for i in range(len(rates) - 1):
		result.extend(linear_interpolation(rates[i], rates[i + 1]))
	result.append(rates[len(rates) - 1])
	date_iterator.date_from_string(result[len(result) - 1][0])
	while more_date(date_iterator, end_date):
		result = result[:-1]
		date_iterator.date_from_string(result[len(result) - 1][0])
	date_iterator.date_from_string(result[len(result) - 1][0])
	while not comp_dates(date_iterator, end_date):
		date_iterator.inc()
		result.append((date_iterator.string_from_date(), rates[len(rates) - 1][1]))
	return result

in_file = open("clipboard_for_downloader.txt", "r")
line_of_tickers = in_file.readline()[:-1]
period = in_file.readline()
in_file.close()
tickers = line_of_tickers.split(",")
tickers = [ticker.strip() for ticker in tickers]
out_file = open("clipboard_for_downloader.txt", "w")
for ticker in tickers:
	if len(ticker) == 0:
		continue;
	if from_finam(ticker):
		rates = download_from_finam(ticker, period)
	else:
		if from_index(ticker):
			ticker = "^" + ticker
		try:
			rates = download_from_yahoo(ticker, period)
		except error.HTTPError:
			rates = []
		if ticker[0] == '^':
			ticker = ticker[1:]
	
	out_file.write("##########\n") 
	out_file.write(ticker + ' ' + period + "\n")
	if len(rates) > 0:
		rates = fill_in_gaps(rates, period)
		for rate in rates:
			out_file.write(rate[0] + " " + str(rate[1]) + "\n")
	else:
		out_file.write("нет данных\n")
	out_file.write("##########\n\n")