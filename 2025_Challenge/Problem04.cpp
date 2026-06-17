import std;

int StringMemory(const std::string& s)
{
	int result = s.size();
	for (char c : s)
		result += c - 'A';
	return result;
}

int DigitSum(int v)
{
	int result = 0;
	while (v > 9)
	{
		result += v % 10;
		v /= 10;
	}
	return result + v;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem04.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0, result2 = 0, result3 = 0;
	std::string line;
	while (in >> line)
	{
		result1 += StringMemory(line);
		size_t stub = line.size() / 10, start = 0;
		result2 += StringMemory(line.substr(0, stub)) + StringMemory(line.substr(line.size() - stub));
		result2 += DigitSum(line.size() - 2 * stub);
		while (start < line.size())
		{
			size_t i = start+1;
			while (i < line.size() && line[start] == line[i])
				++i;
			result3 += DigitSum(i - start) + line[start] - 'A' + 1;
			start = i;
		}
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}