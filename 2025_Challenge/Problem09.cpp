import std;

struct Account
{
	int balance = {};
	std::deque<std::pair<size_t, int>> debts;
	auto operator <=>(const Account&) const = default;
};

void Receive(std::vector<Account>& accounts, size_t to, int amount)
{
	while (!accounts[to].debts.empty() && amount > 0)
	{
		int repay = std::min(accounts[to].debts.front().second, amount);
		size_t debtor = accounts[to].debts.front().first;
		if (repay == accounts[to].debts.front().second)
			accounts[to].debts.pop_front();
		else
			accounts[to].debts.front().second -= repay;
		Receive(accounts, debtor, repay);
		amount -= repay;
	}
	accounts[to].balance += amount;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem09.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0, result2 = 0, result3 = 0, amount;
	std::string line, from, to, _;
	std::unordered_map<std::string, size_t> nameMap;
	std::vector<int> nakedAccounts, limitedAccounts;
	std::vector<Account> accounts;
	while (std::getline(in, line))
	{
		if (line.empty()) break;
		nameMap[line.substr(0, line.find(' '))] = (int)nakedAccounts.size();
		nakedAccounts.push_back(std::stoi(line.substr(line.rfind(' ') + 1)));
	}
	limitedAccounts = nakedAccounts;
	accounts.reserve(nakedAccounts.size());
	for (int i : nakedAccounts)
		accounts.emplace_back(i);

	while (in >> _ >> from >> _ >> to >> _ >> amount)
	{
		size_t f = nameMap[from], t = nameMap[to];
		int limited = std::min(limitedAccounts[f], amount);
		nakedAccounts[f] -= amount;
		nakedAccounts[t] += amount;
		limitedAccounts[f] -= limited;
		limitedAccounts[t] += limited;
		if (accounts[f].balance >= amount)
		{
			accounts[f].balance -= amount;
			Receive(accounts, t, amount);
		}
		else
		{
			int send = accounts[f].balance;
			accounts[f].debts.emplace_back(t, amount - send);
			accounts[f].balance = 0;
			Receive(accounts, t, send);
		}
	}

	std::sort(nakedAccounts.begin(), nakedAccounts.end());
	std::sort(limitedAccounts.begin(), limitedAccounts.end());
	std::sort(accounts.begin(), accounts.end());

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", std::accumulate(nakedAccounts.cend()-3, nakedAccounts.cend(), 0), 
		std::accumulate(limitedAccounts.cend() - 3, limitedAccounts.cend(), 0), 
		std::accumulate(accounts.cend() - 3, accounts.cend(), 0, [](int t, const Account& a) { return t + a.balance; }));
	return 0;
}