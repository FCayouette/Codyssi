import std;

constexpr bool IsAlpha(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return true;
	return false;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem06.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0, result2 = 0, result3 = 0, previousValue = 0;
	std::string line;
	in >> line;
	for (char c : line)
	{
		if (IsAlpha(c))
		{
			++result1;
			if (c >= 'A' && c <= 'Z')
				previousValue = c - 'A' + 27;
			else
				previousValue = c - 'a' + 1;
			result2 += previousValue;
			result3 += previousValue;
		}
		else
		{
			previousValue = previousValue * 2 - 5;
			if (previousValue < 1)
				previousValue += 52;
			else if (previousValue > 52)
				previousValue -= 52;
			result3 += previousValue;
		}
	}
	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}