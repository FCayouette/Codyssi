import std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem07.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0, result3 = 0, index, startIndex;
	std::string line;
	std::vector<int> frequencies;
	while (true)
	{
		std::getline(in, line);
		if (line.empty()) break;
		frequencies.push_back(std::stoi(line));
	}
	std::vector<std::pair<int, int>> swaps;
	while (true)
	{
		std::getline(in, line);
		if (line.empty()) break;
		auto pos = line.find('-');
		swaps.emplace_back(std::stoi(line.substr(0, pos)), std::stoi(line.substr(pos + 1)));
	}
	in >> startIndex;
	result3 = index = startIndex;
	for (auto iter = swaps.crbegin(); iter != swaps.crend(); ++iter)
	{
		auto [low, high] = std::minmax(iter->first, iter->second);
		if (low == index)
			index = high;
		else if (high == index)
			index = low;

		// Part 3
		if (int length = std::min((int)frequencies.size() - high, high - low - 1); result3 >= low && result3 <= low + length)
			result3 += high - low;
		else if (result3 >= high && result3 <= high + length)
			result3 += low - high;
	}
	result1 = frequencies[index - 1];
	index = startIndex;
	for (int i = (int)swaps.size(); i > 0; --i) // part 2
	{
		if (swaps[i % swaps.size()].first == index)
			index = swaps[i - 1].second;
		else if (swaps[i - 1].second==index)
			index = swaps[i - 1].first;
		else if (swaps[i - 1].first == index)
			index = swaps[i % swaps.size()].first;
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, frequencies[index-1], frequencies[result3 - 1]);
	return 0;
}