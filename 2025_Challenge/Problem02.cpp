import std;

using i64 = long long;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem02.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	i64 input, evens = 0, max = 15000000000000LL;
	std::string line;
	auto ReadVal = [&in, &line]()
		{
			std::getline(in, line);
			return std::stoll(line.substr(line.rfind(' ') + 1));
		};
	auto Compute = [add = ReadVal(), mul = ReadVal(), exp = ReadVal()](i64 val)
		{
			i64 result = 1;
			for (int i = 0; i < exp; ++i)
				result *= val;
			return result * mul + add;
		};
	
	std::vector<i64> values;
	while (in >> input)
	{
		if (input % 2 == 0)
			evens += input;
		values.emplace_back(input);
	}
	std::sort(values.begin(), values.end());
	size_t low = 0, high = values.size() - 1, mid = (high + low) / 2;
	while (low != mid)
	{
		if (Compute(values[mid]) <= max)
			low = mid;
		else high = mid;
		mid = (high + low) / 2;
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", Compute(values[values.size() / 2]), Compute(evens), values[mid]);
	return 0;
}