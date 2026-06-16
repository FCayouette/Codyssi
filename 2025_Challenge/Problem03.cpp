import std;

struct Range
{
	int low = -1, high = -1;
	Range() = default;
	Range(std::string s)
	{
		auto pos = s.find('-');
		low = std::stoi(s.substr(0, pos));
		high = std::stoi(s.substr(pos + 1));
	}
	constexpr int Count() const { if (low < 0) return 0; return high - low + 1; }
	constexpr bool operator<(const Range& r) const { if (low < r.low) return true; if (r.low < low) return false; return high < r.high; }
};

void Merge(Range& a, Range& b)
{
	if (b.low <= a.high)
	{
		b.low = a.low;
		b.high = std::max(b.high, a.high);
		a.low = a.high = -1;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem03.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	size_t result1 = 0, result2 = 0, result3 = 0;
	auto ReadPile = [](std::string s)
		{
			auto pos = s.find('-');
			return std::make_pair(std::stoi(s.substr(0, pos)), std::stoi(s.substr(pos + 1)));

		};
	std::string line;
	std::vector<Range> previous, current;
	while (std::getline(in, line))
	{
		auto pos = line.find(' ');
		Range a(line.substr(0, pos)), b(line.substr(pos + 1));
		result1 += a.Count() + b.Count();
		if (a.low > b.low)
			std::swap(a, b);
		Merge(a, b);
		result2 += a.Count() + b.Count();
		if (a.low >= 0)
			current.emplace_back(a);
		current.emplace_back(b);
		for (const Range& r : current)
			previous.push_back(r);
		std::sort(previous.begin(), previous.end());
		for (int i = 1; i < previous.size(); ++i)
			Merge(previous[i - 1], previous[i]);
		size_t total = 0;
		for (const Range& r : previous)
			total += r.Count();
		result3 = std::max(result3, total);

		std::swap(current, previous);
		current.clear();
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}