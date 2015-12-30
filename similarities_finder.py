def diff(s1, s2):
	n = len(s1)
	m = len(s2)
	diff_between_prefixes = []
	for i in range(n + 1):
		diff_between_prefixes.append(([0] * (m + 1)).copy())

	for i in range(m + 1):
		diff_between_prefixes[0][i] = i
	for i in range(n + 1):
		diff_between_prefixes[i][0] = i
	for i in range(1, n + 1):
		for j in range(1, m + 1):
			if s1[i-1] == s2[j-1]:
				diff_between_prefixes[i][j] = diff_between_prefixes[i-1][j-1]
			else:
				diff_between_prefixes[i][j] = min(diff_between_prefixes[i-1][j], diff_between_prefixes[i][j-1], diff_between_prefixes[i - 1][j - 1]) + 1
	return diff_between_prefixes[n][m]


file_finam_ticker_list = open("finam_ticker_list.txt", "r")
file_yahoo_ticker_list = open("yahoo_ticker_list.txt", "r")
clipboard = open("clipboard_for_similarities_finder.txt", "r")
request = clipboard.readline()
clipboard.close()
finam_ticker_list = [(line.split(",")[1][1:-1], line.split(",")[2][1:-1])for line in file_finam_ticker_list]
yahoo_ticker_list = [(line.split(",")[0], line.split(",")[1][:-1]) for line in file_yahoo_ticker_list]
list_of_tips = []
for stock in finam_ticker_list:
	if (len(request) > 1 and request in stock[1]) or (len(request) < 7 and diff(stock[0], request) < 2):
		list_of_tips.append(stock)
for stock in yahoo_ticker_list:
	if (len(request) > 1 and request in stock[1]) or (len(request) < 7 and diff(stock[0], request) < 2):
		list_of_tips.append(stock)
clipboard = open("clipboard_for_similarities_finder.txt", "w")
for tip in list_of_tips:
	clipboard.write(tip[0] + " - " + tip[1] + "\n")
clipboard.close()