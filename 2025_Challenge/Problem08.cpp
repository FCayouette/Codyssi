import std;

constexpr bool IsAlpha(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return true;
	return false;
}

constexpr bool IsDigit(char c)
{
	return c >= '0' && c <= '9';
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage: Problem08.exe inputFilename\n";
		return -1;
	}
	std::ifstream in(argv[1]);
	if (!in)
	{
		std::cout << std::format("Could not open {}\n", argv[1]);
		return 1;
	}

	int result1 = 0, result2 = 0, result3 = 0;
	std::string line, reduced;
	while (in >> line)
	{
		// Part 1 and 3
		for (char c : line)
		{
			result1 += IsAlpha(c);
			if (!reduced.empty())
			{
				if (char b = reduced.back(); (IsDigit(c) && IsAlpha(b)) || (IsAlpha(c) && IsDigit(b)))
					reduced.pop_back();
				else reduced.push_back(c);
			}
			else reduced.push_back(c);
		}

		result3 += reduced.size();
		line.clear();
		
		for (char c : reduced) // Part 2
		{
			if (!line.empty())
			{
				if (char b = line.back(); IsDigit(c))
				{
					if (b == '-' || IsAlpha(b))
						line.pop_back();
					else line.push_back(c);
				}
				else
				{
					if (IsDigit(b))
						line.pop_back();
					else line.push_back(c);
				}
			}
			else
				line.push_back(c);
		}
		result2 += line.size();
		reduced.clear();
	}

	std::cout << std::format("Part 1: {}\nPart 2: {}\nPart 3: {}\n", result1, result2, result3);
	return 0;
}